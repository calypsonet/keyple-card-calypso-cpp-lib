/**************************************************************************************************
 * Copyright (c) 2021 Calypso Networks Association https://calypsonet.org/                        *
 *                                                                                                *
 * See the NOTICE file(s) distributed with this work for additional information regarding         *
 * copyright ownership.                                                                           *
 *                                                                                                *
 * This program and the accompanying materials are made available under the terms of the Eclipse  *
 * Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                  *
 *                                                                                                *
 * SPDX-License-Identifier: EPL-2.0                                                               *
 **************************************************************************************************/

#pragma once

#include <memory>

/* Calypsonet Terminal Card */
#include "ApduResponseApi.h"

/* Keyple Card Calypso */
#include "AbstractCardCommand.h"
#include "CalypsoCardAdapter.h"
#include "CmdCardAppendRecord.h"
#include "CmdCardIncreaseOrDecrease.h"
#include "CmdCardIncreaseOrDecreaseMultiple.h"
#include "CmdCardOpenSession.h"
#include "CmdCardReadRecords.h"
#include "CmdCardGetDataEfList.h"
#include "CmdCardGetDataTraceabilityInformation.h"
#include "CmdCardReadRecordMultiple.h"
#include "CmdCardSearchRecordMultiple.h"
#include "CmdCardUpdateRecord.h"
#include "CmdCardWriteRecord.h"

namespace keyple {
namespace card {
namespace calypso {

using namespace calypsonet::terminal::card;

/**
 * (package-private)<br>
 * Helper class used to update the CalypsoCard with the
 * responses received from the card.
 *
 * @since 2.0.0
 */
class CalypsoCardUtilAdapter final {
public:
    /**
     * (package-private)<br>
     * Fills the CalypsoCard with the card's response to a single command
     *
     * @param calypsoCard the CalypsoCardAdapter object to fill with the provided response from the
     *        card.
     * @param command the command that get the response.
     * @param apduResponse the APDU response returned by the card to the command.
     * @param isSessionOpen true when a secure session is open.
     * @throws CardCommandException if a response from the card was unexpected
     * @since 2.0.0
     */
    static void updateCalypsoCard(std::shared_ptr<CalypsoCardAdapter> calypsoCard,
                                  const std::shared_ptr<AbstractCardCommand> command,
                                  const std::shared_ptr<ApduResponseApi> apduResponse,
                                  const bool isSessionOpen);

    /**
     * (package-private)<br>
     * Fills the CalypsoCard with the card's responses to a list of commands
     *
     * @param calypsoCard the CalypsoCardAdapter object to fill with the provided response from the
     *        card
     * @param commands the list of commands that get the responses.
     * @param apduResponses the APDU responses returned by the card to all commands.
     * @param isSessionOpen true when a secure session is open.
     * @throw CardCommandException if a response from the card was unexpected
     * @since 2.0.0
     */
    static void updateCalypsoCard(
        std::shared_ptr<CalypsoCardAdapter> calypsoCard,
        const std::vector<std::shared_ptr<AbstractCardCommand>>& commands,
        const std::vector<std::shared_ptr<ApduResponseApi>>& apduResponses,
        const bool isSessionOpen);

private:
    /**
     * Private constructor
     */
    CalypsoCardUtilAdapter();

    /**
     * (private)<br>
     * Updates the {@link CalypsoCardAdapter} object with the response to an Open Secure Session
     * command received from the card <br>
     * The ratification status and the data read at the time of the session opening are added to the
     * CalypsoCard.
     *
     * @param calypsoCard the {@link CalypsoCardAdapter} object to update.
     * @param cmdCardOpenSession the command.
     * @param apduResponse the response received.
     */
    static void updateCalypsoCardOpenSession(
        std::shared_ptr<CalypsoCardAdapter> calypsoCard,
        std::shared_ptr<CmdCardOpenSession> cmdCardOpenSession,
        const std::shared_ptr<ApduResponseApi> apduResponse);

    // /**
    //  * (private)<br>
    //  * Checks the response to a Close Session command
    //  *
    //  * @param cmdCardCloseSession the command.
    //  * @param apduResponse the response received.
    //  * @throws CardCommandException if a response from the card was unexpected
    //  */
    // private static void updateCalypsoCardCloseSession(
    //     CmdCardCloseSession cmdCardCloseSession, ApduResponseApi apduResponse)
    //     throws CardCommandException {
    //     cmdCardCloseSession.setApduResponse(apduResponse).checkStatus();
    // }

    /**
     * (private)<br>
     * Updates the CalypsoCardAdapter object with the response to a 'Read Records' command
     * received from the card.<br>
     * The records read are added to the CalypsoCardAdapter file structure.
     *
     * @param calypsoCard the {@link CalypsoCardAdapter} object to update.
     * @param cmdCardReadRecords the command.
     * @param apduResponse the response received.
     * @param isSessionOpen true when a secure session is open.
     * @throw CardCommandException if a response from the card was unexpected
     */
    static void updateCalypsoCardReadRecords(
        const std::shared_ptr<CalypsoCardAdapter> calypsoCard,
        const std::shared_ptr<CmdCardReadRecords> cmdCardReadRecords,
        const std::shared_ptr<ApduResponseApi> apduResponse,
        const bool isSessionOpen);

    /**
     * (private)<br>
     * Sets the response to the command and check the status for strict and best effort mode.
     *
     * @param command The command.
     * @param isSessionOpen Is session open?
     * @throw CardCommandException If needed.
     */
    static void checkResponseStatusForStrictAndBestEffortMode(
        const std::shared_ptr<AbstractCardCommand> command, const bool isSessionOpen);

    /**
     * (private)<br>
     * Updates the SearchCommandDataAdapter and the CalypsoCardAdapter objects with
     * the response to a "Search Record Multiple" command received from the card.<br>
     * The first matching record content is added to the CalypsoCardAdapter file structure if
     * requested.
     *
     * @param calypsoCard The CalypsoCardAdapter object to update.
     * @param cmdCardSearchRecordMultiple The command.
     * @param apduResponse The response received.
     * @param isSessionOpen True when a secure session is open.
     * @throw CardCommandException If a response from the card was unexpected.
     */
    static void updateCalypsoCardSearchRecordMultiple(
        std::shared_ptr<CalypsoCardAdapter> calypsoCard,
        std::shared_ptr<CmdCardSearchRecordMultiple> cmdCardSearchRecordMultiple,
        const std::shared_ptr<ApduResponseApi> apduResponse,
        const bool isSessionOpen);

    /**
     * (private)<br>
     * Updates the CalypsoCardAdapter object with the response to a 'Read Record Multiple'
     * command received from the card.<br>
     * The records read are added to the CalypsoCardAdapter file structure.
     *
     * @param calypsoCard The {@link CalypsoCardAdapter} object to update.
     * @param cmdCardReadRecordMultiple The command.
     * @param apduResponse The response received.
     * @param isSessionOpen True when a secure session is open.
     * @throw CardCommandException If a response from the card was unexpected.
     */
    static void updateCalypsoCardReadRecordMultiple(
        std::shared_ptr<CalypsoCardAdapter> calypsoCard,
        std::shared_ptr<CmdCardReadRecordMultiple> cmdCardReadRecordMultiple,
        const std::shared_ptr<ApduResponseApi> apduResponse,
        const bool isSessionOpen);

    // /**
    //  * (private)<br>
    //  * Updates the {@link CalypsoCardAdapter} object with the response to a 'Read Binary' command
    //  * received from the card.<br>
    //  * The records read are added to the {@link CalypsoCardAdapter} file structure.
    //  *
    //  * @param calypsoCard the {@link CalypsoCardAdapter} object to update.
    //  * @param cmdCardReadBinary the command.
    //  * @param apduResponse the response received.
    //  * @param isSessionOpen true when a secure session is open.
    //  * @throws CardCommandException if a response from the card was unexpected
    //  */
    // private static void updateCalypsoCardReadBinary(
    //     CalypsoCardAdapter calypsoCard,
    //     CmdCardReadBinary cmdCardReadBinary,
    //     ApduResponseApi apduResponse,
    //     boolean isSessionOpen)
    //     throws CardCommandException {

    //     cmdCardReadBinary.setApduResponse(apduResponse);
    //     checkResponseStatusForStrictAndBestEffortMode(cmdCardReadBinary, isSessionOpen);

    //     calypsoCard.setContent(
    //         cmdCardReadBinary.getSfi(), 1, apduResponse.getDataOut(), cmdCardReadBinary.getOffset());
    // }

    /**
     * (private)<br>
     * Updates the {@link CalypsoCardAdapter} object with the response to a Select File command
     * received from the card.<br>
     * Depending on the content of the response, either a {@link FileHeaderAdapter} is added or the
     * {@link DirectoryHeaderAdapter} is updated
     *
     * @param calypsoCard The {@link CalypsoCardAdapter} object to update.
     * @param command The command.
     * @param apduResponse The response received.
     * @throw CardCommandException If a response from the card was unexpected.
     */
    static void updateCalypsoCardWithFcp(
        std::shared_ptr<CalypsoCardAdapter> calypsoCard,
        std::shared_ptr<AbstractCardCommand> command,
        const std::shared_ptr<ApduResponseApi> apduResponse);

    /**
     * (private)<br>
     * Updates the CalypsoCardAdapter object with the response to a "Get Data" command for
     * GetDataTag::EF_LIST tag received from the card.
     *
     * <p>Non-existing file headers will be created for each received descriptor. Existing file
     * headers will remain unchanged.
     *
     * @param calypsoCard The CalypsoCardAdapter object to update.
     * @param command The command.
     * @param apduResponse The response received.
     * @throw CardCommandException If a response from the card was unexpected.
     */
    static void updateCalypsoCardWithEfList(
        std::shared_ptr<CalypsoCardAdapter> calypsoCard,
        std::shared_ptr<CmdCardGetDataEfList> command,
        const std::shared_ptr<ApduResponseApi> apduResponse);

    /**
     * (private)<br>
     * Updates the CalypsoCardAdapter object with the response to a "Get Data" command for
     * GetDataTag::TRACEABILITY_INFORMATION tag received from
     * the card.
     *
     * @param calypsoCard The CalypsoCardAdapter object to update.
     * @param command The command.
     * @param apduResponse The response received.
     * @throw CardCommandException if a response from the card was unexpected.
     */
    static void updateCalypsoCardWithTraceabilityInformation(
        std::shared_ptr<CalypsoCardAdapter> calypsoCard,
        std::shared_ptr<CmdCardGetDataTraceabilityInformation> command,
        const std::shared_ptr<ApduResponseApi> apduResponse);

    /**
     * (private)<br>
     * Updates the {@link CalypsoCardAdapter} object with the response to an "Update Record" command
     * sent and received from the card.
     *
     * @param calypsoCard the {@link CalypsoCardAdapter} object to update.
     * @param cmdCardUpdateRecord the command.
     * @param apduResponse the response received.
     * @throw CardCommandException if a response from the card was unexpected
     */
    static void updateCalypsoCardUpdateRecord(
        std::shared_ptr<CalypsoCardAdapter> calypsoCard,
        std::shared_ptr<CmdCardUpdateRecord> cmdCardUpdateRecord,
        const std::shared_ptr<ApduResponseApi> apduResponse);

    /**
     * (private)<br>
     * Updates the CalypsoCardAdapter object with the response to a "Write Record" command
     * sent and received from the card.
     *
     * @param calypsoCard the CalypsoCardAdapter object to update.
     * @param cmdCardWriteRecord the command.
     * @param apduResponse the response received.
     * @throws CardCommandException if a response from the card was unexpected
     */
    static void updateCalypsoCardWriteRecord(
        std::shared_ptr<CalypsoCardAdapter> calypsoCard,
        std::shared_ptr<CmdCardWriteRecord> cmdCardWriteRecord,
        const std::shared_ptr<ApduResponseApi> apduResponse);

    // /**
    //  * (private)<br>
    //  * Updates the {@link CalypsoCardAdapter} object with the response to an "Update Binary" command
    //  * sent and received from the card.
    //  *
    //  * @param calypsoCard the {@link CalypsoCardAdapter} object to update.
    //  * @param cmdCardUpdateBinary the command.
    //  * @param apduResponse the response received.
    //  * @throws CardCommandException if a response from the card was unexpected
    //  */
    // private static void updateCalypsoCardUpdateBinary(
    //     CalypsoCardAdapter calypsoCard,
    //     CmdCardUpdateOrWriteBinary cmdCardUpdateBinary,
    //     ApduResponseApi apduResponse)
    //     throws CardCommandException {

    //     cmdCardUpdateBinary.setApduResponse(apduResponse).checkStatus();

    //     calypsoCard.setContent(
    //         cmdCardUpdateBinary.getSfi(),
    //         1,
    //         cmdCardUpdateBinary.getData(),
    //         cmdCardUpdateBinary.getOffset());
    // }

    // /**
    //  * (private)<br>
    //  * Updates the {@link CalypsoCardAdapter} object with the response to a "Write Binary" command
    //  * sent and received from the card.
    //  *
    //  * @param calypsoCard the {@link CalypsoCardAdapter} object to update.
    //  * @param cmdCardWriteBinary the command.
    //  * @param apduResponse the response received.
    //  * @throws CardCommandException if a response from the card was unexpected
    //  */
    // private static void updateCalypsoCardWriteBinary(
    //     CalypsoCardAdapter calypsoCard,
    //     CmdCardUpdateOrWriteBinary cmdCardWriteBinary,
    //     ApduResponseApi apduResponse)
    //     throws CardCommandException {

    //     cmdCardWriteBinary.setApduResponse(apduResponse).checkStatus();

    //     calypsoCard.fillContent(
    //         cmdCardWriteBinary.getSfi(),
    //         1,
    //         cmdCardWriteBinary.getData(),
    //         cmdCardWriteBinary.getOffset());
    // }

    /**
     * (private)<br>
     * Updates the {@link CalypsoCardAdapter} object with the response to a Read Records command
     * received from the card.<br>
     * The records read are added to the {@link CalypsoCardAdapter} file structure.
     *
     * @param calypsoCard the {@link CalypsoCardAdapter} object to update.
     * @param cmdCardAppendRecord the command.
     * @param apduResponse the response received.
     * @throw CardCommandException if a response from the card was unexpected
     */
    static void updateCalypsoCardAppendRecord(
        std::shared_ptr<CalypsoCardAdapter> calypsoCard,
        std::shared_ptr<CmdCardAppendRecord> cmdCardAppendRecord,
        const std::shared_ptr<ApduResponseApi> apduResponse);

    /**
     * (private)<br>
     * Updates the {@link CalypsoCardAdapter} object with the response to a Decrease/Increase command
     * received from the card <br>
     * The counter value is updated in the {@link CalypsoCardAdapter} file structure.
     *
     * @param calypsoCard the {@link CalypsoCardAdapter} object to update.
     * @param cmdCardIncreaseOrDecrease the command.
     * @param apduResponse the response received.
     * @throw CardCommandException if a response from the card was unexpected
     */
    static void updateCalypsoCardIncreaseOrDecrease(
        std::shared_ptr<CalypsoCardAdapter> calypsoCard,
        std::shared_ptr<CmdCardIncreaseOrDecrease> cmdCardIncreaseOrDecrease,
        const std::shared_ptr<ApduResponseApi> apduResponse);

    /**
     * (private)<br>
     * Updates the {@link CalypsoCardAdapter} object with the response to a Decrease/Increase Multiple
     * command received from the card <br>
     * The counter value is updated in the {@link CalypsoCardAdapter} file structure.
     *
     * @param calypsoCard the {@link CalypsoCardAdapter} object to update.
     * @param cmdCardIncreaseOrDecreaseMultiple the command.
     * @param apduResponse the response received.
     * @throw CardCommandException if a response from the card was unexpected
     */
    static void updateCalypsoCardIncreaseOrDecreaseMultiple(
        std::shared_ptr<CalypsoCardAdapter> calypsoCard,
        std::shared_ptr<CmdCardIncreaseOrDecreaseMultiple> cmdCardIncreaseOrDecreaseMultiple,
        const std::shared_ptr<ApduResponseApi> apduResponse);

    // /**
    //  * (private)<br>
    //  * Parses the response to a Get Challenge command received from the card.<br>
    //  * The card challenge value is stored in {@link CalypsoCardAdapter}.
    //  *
    //  * @param calypsoCard the {@link CalypsoCardAdapter} object to update.
    //  * @param cmdCardGetChallenge the command.
    //  * @param apduResponse the response received.
    //  * @throws CardCommandException if a response from the card was unexpected
    //  */
    // private static void updateCalypsoCardGetChallenge(
    //     CalypsoCardAdapter calypsoCard,
    //     CmdCardGetChallenge cmdCardGetChallenge,
    //     ApduResponseApi apduResponse)
    //     throws CardCommandException {

    //     cmdCardGetChallenge.setApduResponse(apduResponse).checkStatus();
    //     calypsoCard.setCardChallenge(cmdCardGetChallenge.getCardChallenge());
    // }

    // /**
    //  * (private)<br>
    //  * Updates the {@link CalypsoCardAdapter} object with the response to a "Verify Pin" command
    //  * received from the card.<br>
    //  * The PIN attempt counter value is stored in the {@link CalypsoCardAdapter}<br>
    //  * CardPinException are filtered when the initial command targets the reading of the attempt
    //  * counter.
    //  *
    //  * @param calypsoCard the {@link CalypsoCardAdapter} object to update.
    //  * @param cmdCardVerifyPin the command.
    //  * @param apduResponse the response received.
    //  * @throws CardCommandException if a response from the card was unexpected
    //  */
    // private static void updateCalypsoVerifyPin(
    //     CalypsoCardAdapter calypsoCard,
    //     CmdCardVerifyPin cmdCardVerifyPin,
    //     ApduResponseApi apduResponse)
    //     throws CardCommandException {

    //     cmdCardVerifyPin.setApduResponse(apduResponse);
    //     calypsoCard.setPinAttemptRemaining(cmdCardVerifyPin.getRemainingAttemptCounter());

    //     try {
    //     cmdCardVerifyPin.checkStatus();
    //     } catch (CardPinException ex) {
    //     // forward the exception if the operation do not target the reading of the attempt
    //     // counter.
    //     // catch it silently otherwise
    //     if (!cmdCardVerifyPin.isReadCounterOnly()) {
    //         throw ex;
    //     }
    //     }
    // }

    // /**
    //  * (private)<br>
    //  * Checks the status of the response to a "Change Pin" command received from the card
    //  *
    //  * @param cmdCardChangePin the command.
    //  * @param apduResponse the response received.
    //  */
    // private static void updateCalypsoChangePin(
    //     CmdCardChangePin cmdCardChangePin, ApduResponseApi apduResponse) throws CardCommandException {
    //     cmdCardChangePin.setApduResponse(apduResponse).checkStatus();
    // }

    // /**
    //  * (private)<br>
    //  * Checks the status of the response to a "Change Key" command received from the card
    //  *
    //  * @param cmdCardChangeKey the command.
    //  * @param apduResponse the response received.
    //  */
    // private static void updateCalypsoChangeKey(
    //     CmdCardChangeKey cmdCardChangeKey, ApduResponseApi apduResponse) throws CardCommandException {
    //     cmdCardChangeKey.setApduResponse(apduResponse).checkStatus();
    // }

    // /**
    //  * (private)<br>
    //  * Updates the {@link CalypsoCardAdapter} object with the response to an SV Get command received
    //  * from the card <br>
    //  * The SV Data values (KVC, command header, response data) are stored in {@link
    //  * CalypsoCardUtilAdapter} and made available through a dedicated getters for later use<br>
    //  *
    //  * @param calypsoCard the {@link CalypsoCardAdapter} object to update.
    //  * @param cmdCardSvGet the command.
    //  * @param apduResponse the response received.
    //  * @throws CardCommandException if a response from the card was unexpected
    //  */
    // private static void updateCalypsoCardSvGet(
    //     CalypsoCardAdapter calypsoCard, CmdCardSvGet cmdCardSvGet, ApduResponseApi apduResponse)
    //     throws CardCommandException {

    //     cmdCardSvGet.setApduResponse(apduResponse).checkStatus();

    //     calypsoCard.setSvData(
    //         cmdCardSvGet.getCurrentKVC(),
    //         cmdCardSvGet.getSvGetCommandHeader(),
    //         cmdCardSvGet.getApduResponse().getApdu(),
    //         cmdCardSvGet.getBalance(),
    //         cmdCardSvGet.getTransactionNumber(),
    //         cmdCardSvGet.getLoadLog(),
    //         cmdCardSvGet.getDebitLog());
    // }

    // /**
    //  * (private)<br>
    //  * Checks the response to a SV Operation command (reload, debit or undebit) response received from
    //  * the card<br>
    //  * Stores the card SV signature if any (command executed outside a secure session) in the {@link
    //  * CalypsoCardAdapter}.
    //  *
    //  * @param calypsoCard the {@link CalypsoCardAdapter} object to update.
    //  * @param cmdCardSvOperation the SV Operation command (CmdCardSvReload, CmdCardSvDebit or
    //  *     CmdCardSvUndebit)
    //  * @param apduResponse the response received.
    //  * @throws CardCommandException if a response from the card was unexpected
    //  */
    // private static void updateCalypsoCardSvOperation(
    //     CalypsoCardAdapter calypsoCard,
    //     AbstractCardCommand cmdCardSvOperation,
    //     ApduResponseApi apduResponse)
    //     throws CardCommandException {

    //     cmdCardSvOperation.setApduResponse(apduResponse).checkStatus();
    //     calypsoCard.setSvOperationSignature(cmdCardSvOperation.getApduResponse().getDataOut());
    // }

    // /**
    //  * (private)<br>
    //  * Checks the response to Invalidate/Rehabilitate commands
    //  *
    //  * @param cmdCardInvalidateRehabilitate the Invalidate or Rehabilitate command.
    //  * @param apduResponse the response received.
    //  * @throws CardCommandException if a response from the card was unexpected
    //  */
    // private static void updateCalypsoInvalidateRehabilitate(
    //     AbstractCardCommand cmdCardInvalidateRehabilitate, ApduResponseApi apduResponse)
    //     throws CardCommandException {
    //     cmdCardInvalidateRehabilitate.setApduResponse(apduResponse).checkStatus();
    // }

    /**
     * (private)<br>
     * Parses the proprietaryInformation field of a file identified as an DF and create a {@link
     * DirectoryHeader}
     *
     * @param proprietaryInformation from the response to a Select File command.
     * @return A DirectoryHeader object
     */
    static const std::shared_ptr<DirectoryHeader> createDirectoryHeader(
        const std::vector<uint8_t>& proprietaryInformation);

    /**
     * (private)<br>
     * Converts the EF type value from the card into a ElementaryFile::Type enum
     *
     * @param efType the value returned by the card.
     * @return The corresponding ElementaryFile::Type
     */
    static ElementaryFile::Type getEfTypeFromCardValue(const uint8_t efType);

    /**
     * (private)<br>
     * Parses the proprietaryInformation field of a file identified as an EF and create a
     * FileHeaderAdapter
     *
     * @param proprietaryInformation from the response to a Select File command.
     * @return A FileHeaderAdapter object
     */
    static const std::shared_ptr<FileHeaderAdapter> createFileHeader(
        const std::vector<uint8_t>& proprietaryInformation);
};

}
}
}
