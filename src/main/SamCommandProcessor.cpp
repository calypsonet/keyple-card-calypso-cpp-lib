/**************************************************************************************************
 * Copyright (c) 2022 Calypso Networks Association https://calypsonet.org/                        *
 *                                                                                                *
 * See the NOTICE file(s) distributed with this work for additional information regarding         *
 * copyright ownership.                                                                           *
 *                                                                                                *
 * This program and the accompanying materials are made available under the terms of the Eclipse  *
 * Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                  *
 *                                                                                                *
 * SPDX-License-Identifier: EPL-2.0                                                               *
 **************************************************************************************************/

#include "SamCommandProcessor.h"

/* Calypsonet Terminal Calypso */
#include "DesynchronizedExchangesException.h"

/* Calypsonet Terminal Card */
#include "CardSecuritySettingAdapter.h"
#include "UnexpectedStatusWordException.h"

/* Keyple Card Calypso */
#include "ApduRequestSpi.h"
#include "CmdSamDigestClose.h"
#include "CmdSamDigestInit.h"
#include "CmdSamDigestUpdate.h"
#include "CmdSamGetChallenge.h"
#include "CmdSamSelectDiversifier.h"

/* Keyple Card Generic */
#include "CardRequestAdapter.h"

/* Keyple Core Util */
#include "ApduUtil.h"
#include "Arrays.h"
#include "ByteArrayUtil.h"
#include "IllegalStateException.h"
#include "KeypleAssert.h"

namespace keyple {
namespace card {
namespace calypso {

using namespace calypsonet::terminal::calypso::transaction;
using namespace calypsonet::terminal::card::spi;
using namespace keyple::card::calypso;
using namespace keyple::card::generic;
using namespace keyple::core::util;
using namespace keyple::core::util::cpp;
using namespace keyple::core::util::cpp::exception;

const uint8_t SamCommandProcessor::KIF_UNDEFINED = 0xFF;
const uint8_t SamCommandProcessor::CHALLENGE_LENGTH_REV_INF_32 = 0x04;
const uint8_t SamCommandProcessor::CHALLENGE_LENGTH_REV32 = 0x08;
const uint8_t SamCommandProcessor::SIGNATURE_LENGTH_REV_INF_32 = 0x04;
const uint8_t SamCommandProcessor::SIGNATURE_LENGTH_REV32 = 0x08;
const std::string SamCommandProcessor::UNEXPECTED_EXCEPTION = "An unexpected exception was raised.";

SamCommandProcessor::SamCommandProcessor(
  const std::shared_ptr<CalypsoCard> calypsoCard,
  const std::shared_ptr<CardSecuritySetting> cardSecuritySetting)
: mCardSecuritySettings(cardSecuritySetting),
  mCalypsoCard(std::dynamic_pointer_cast<CalypsoCardAdapter>(calypsoCard))
{
    const auto stngs = std::dynamic_pointer_cast<CardSecuritySettingAdapter>(cardSecuritySetting);
    Assert::getInstance().notNull(stngs, "securitySettings")
                         .notNull(stngs->getSamReader(), "samReader")
                         .notNull(stngs->getCalypsoSam(), "calypsoSam");

    const auto calypsoSam = stngs->getCalypsoSam();
    mSamProductType = calypsoSam->getProductType();
    mSamSerialNumber = calypsoSam->getSerialNumber();
    mSamReader = std::dynamic_pointer_cast<ProxyReaderApi>(stngs->getSamReader());
}

const std::vector<uint8_t> SamCommandProcessor::getSessionTerminalChallenge()
{
    std::vector<std::shared_ptr<ApduRequestSpi>> apduRequests;

    /* Diversify only if this has not already been done */
    if (!mIsDiversificationDone) {
        /*
         * Build the SAM Select Diversifier command to provide the SAM with the card S/N
         * CL-SAM-CSN.1
         */
        const auto selectDiversifierCmd =
            std::make_shared<CmdSamSelectDiversifier>(mSamProductType,
                                                      mCalypsoCard->getCalypsoSerialNumberFull());

        apduRequests.push_back(selectDiversifierCmd->getApduRequest());

        /* Note that the diversification has been made */
        mIsDiversificationDone = true;
    }

    /* Build the SAM Get Challenge command */
    const uint8_t challengeLength = mCalypsoCard->isExtendedModeSupported() ?
                                    CHALLENGE_LENGTH_REV32 : CHALLENGE_LENGTH_REV_INF_32;

    auto samGetChallengeCmd = std::make_shared<CmdSamGetChallenge>(mSamProductType,challengeLength);

    apduRequests.push_back(samGetChallengeCmd->getApduRequest());

    /* Transmit the CardRequest to the SAM and get back the CardResponse (list of ApduResponseApi)*/
    std::shared_ptr<CardResponseApi> samCardResponse;
    try {
        samCardResponse = mSamReader->transmitCardRequest(
                              std::make_shared<CardRequestAdapter>(apduRequests, false),
                              ChannelControl::KEEP_OPEN);
    } catch (const UnexpectedStatusWordException& e) {
        throw IllegalStateException(UNEXPECTED_EXCEPTION,
                                    std::make_shared<UnexpectedStatusWordException>(e));
    }

    const std::vector<std::shared_ptr<ApduResponseApi>>&
        samApduResponses = samCardResponse->getApduResponses();
    std::vector<uint8_t> sessionTerminalChallenge;

    const int numberOfSamCmd = apduRequests.size();
    if (static_cast<int>(samApduResponses.size()) == numberOfSamCmd) {
        samGetChallengeCmd->setApduResponse(samApduResponses[numberOfSamCmd - 1]).checkStatus();
        sessionTerminalChallenge = samGetChallengeCmd->getChallenge();
        mLogger->debug("identification: TERMINALCHALLENGE = %\n",
                       ByteArrayUtil::toHex(sessionTerminalChallenge));

    } else {
        throw DesynchronizedExchangesException("The number of commands/responses does not match: " \
                                               "cmd=" + std::to_string(numberOfSamCmd) + ", " +
                                               "resp=" + std::to_string(samApduResponses.size()));
    }

    return sessionTerminalChallenge;
}

const std::shared_ptr<uint8_t> SamCommandProcessor::computeKvc(
    const WriteAccessLevel writeAccessLevel, const std::shared_ptr<uint8_t> kvc) const
{
    if (kvc != nullptr) {
        return kvc;
    }

    return std::dynamic_pointer_cast<CardSecuritySettingAdapter>(mCardSecuritySettings)
               ->getDefaultKvc(writeAccessLevel);
}

const std::shared_ptr<uint8_t> SamCommandProcessor::computeKif(
    const WriteAccessLevel writeAccessLevel,
    const std::shared_ptr<uint8_t> kif,
    const std::shared_ptr<uint8_t> kvc)
{
    /* CL-KEY-KIF.1 */
    if ((kif != nullptr && *kif.get() != KIF_UNDEFINED) || (kvc == nullptr)) {
        return kif;
    }

    /* CL-KEY-KIFUNK.1 */
    const auto adptr = std::dynamic_pointer_cast<CardSecuritySettingAdapter>(mCardSecuritySettings);
    std::shared_ptr<uint8_t> result = adptr->getKif(writeAccessLevel, *kvc.get());
    if (result == nullptr) {
        result = adptr->getDefaultKif(writeAccessLevel);
    }

    return result;
}

void SamCommandProcessor::initializeDigester(const bool sessionEncryption,
                                             const bool verificationMode,
                                             const uint8_t kif,
                                             const uint8_t kvc,
                                             const std::vector<uint8_t>& digestData)
{
    mSessionEncryption = sessionEncryption;
    mVerificationMode = verificationMode;
    mKif = kif;
    mKvc = kvc;

    mLogger->debug("initialize: POREVISION = %, SAMREVISION = %, SESSIONENCRYPTION = %, " \
                   "VERIFICATIONMODE = %\n",
                   mCalypsoCard->getProductType(),
                   mSamProductType,
                   sessionEncryption,
                   verificationMode);
    mLogger->debug("initialize: VERIFICATIONMODE = %, REV32MODE = %\n",
                   verificationMode,
                   mCalypsoCard->isExtendedModeSupported());
    mLogger->debug("initialize: KIF = %, KVC %, DIGESTDATA = %\n",
                   kif,
                   kvc,
                   ByteArrayUtil::toHex(digestData));

    /* Clear data cache */
    mCardDigestDataCache.clear();

    /* Build Digest Init command as first ApduRequestAdapter of the digest computation process */
    mCardDigestDataCache.push_back(digestData);

    mIsDigestInitDone = false;
    mIsDigesterInitialized = true;
}

void SamCommandProcessor::pushCardExchangedData(const std::shared_ptr<ApduRequestSpi> request,
                                                const std::shared_ptr<ApduResponseApi> response)
{
    mLogger->trace("pushCardExchangedData: %\n", request);

    /*
     * Add an ApduRequestAdapter to the digest computation: if the request is of case4 type, Le must
     * be excluded from the digest computation. In this cas, we remove here the last byte of the
     * command buffer.
     * CL-C4-MAC.1
     */
    if (ApduUtil::isCase4(request->getApdu())) {
        mCardDigestDataCache.push_back(
            Arrays::copyOfRange(request->getApdu(), 0, request->getApdu().size() - 1));
    } else {
        mCardDigestDataCache.push_back(request->getApdu());
    }

    mLogger->trace("pushCardExchangedData: %\n", response);

    /* Add an ApduResponseApi to the digest computation */
    mCardDigestDataCache.push_back(response->getApdu());
}

void SamCommandProcessor::pushCardExchangedData(
    const std::vector<std::shared_ptr<ApduRequestSpi>>& requests,
    const std::vector<std::shared_ptr<ApduResponseApi>>& responses,
    const int startIndex)
{
    for (int i = startIndex; i < static_cast<int>(requests.size()); i++) {
        /* Add requests and responses to the digest processor */
        pushCardExchangedData(requests[i], responses[i]);
    }
}

const std::vector<std::shared_ptr<AbstractSamCommand>> SamCommandProcessor::getPendingSamCommands(
    const bool addDigestClose)
{
    /* TODO optimization with the use of Digest Update Multiple whenever possible */
    std::vector<std::shared_ptr<AbstractSamCommand>> samCommands;

    /* Sanity checks */
    if (mCardDigestDataCache.empty()) {
        mLogger->debug("getSamDigestRequest: no data in cache\n");
        throw IllegalStateException("Digest data cache is empty.");
    }

    if (!mIsDigestInitDone && mCardDigestDataCache.size() % 2 == 0) {
        /* The number of buffers should be 2*n + 1 */
        mLogger->debug("getSamDigestRequest: wrong number of buffer in cache NBR = %\n",
                       mCardDigestDataCache.size());
        throw IllegalStateException("Digest data cache is inconsistent.");
    }

    if (!mIsDigestInitDone) {
        /*
         * Build and append Digest Init command as first ApduRequestAdapter of the digest
         * computation process. The Digest Init command comes from the Open Secure Session response
         * from the card. Once added to the ApduRequestAdapter list, the data is remove from the
         * cache to keep only couples of card request/response
         * CL-SAM-DINIT.1
         */
        samCommands.push_back(
            std::make_shared<CmdSamDigestInit>(mSamProductType,
                                               mVerificationMode,
                                               mCalypsoCard->isExtendedModeSupported(),
                                               mKif,
                                               mKvc,
                                               mCardDigestDataCache[0]));
        mCardDigestDataCache.erase(mCardDigestDataCache.begin());

        /* Note that the digest init has been made */
        mIsDigestInitDone = true;
    }

    /*
     * Build and append Digest Update commands
     * CL-SAM-DUPDATE.1
     */
    for (const auto& bytes : mCardDigestDataCache) {
        samCommands.push_back(
            std::make_shared<CmdSamDigestUpdate>(mSamProductType, mSessionEncryption, bytes));
    }

    /* Clears cached commands once they have been processed */
    mCardDigestDataCache.clear();

    if (addDigestClose) {
        /*
         * Build and append Digest Close command
         * CL-SAM-DCLOSE.1
         */
        samCommands.push_back(
            std::make_shared<CmdSamDigestClose>(mSamProductType,
                                                mCalypsoCard->isExtendedModeSupported() ?
                                                SIGNATURE_LENGTH_REV32 :
                                                SIGNATURE_LENGTH_REV_INF_32));
    }

    return samCommands;
}

}
}
}
