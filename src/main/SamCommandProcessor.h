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

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/* Calypsonet Terminal Calypso */
#include "CalypsoSam.h"
#include "CardSecuritySetting.h"

/* Calypsonet Terminal Card */
#include "ProxyReaderApi.h"

/* Keyple Card Calypso */
#include "AbstractSamCommand.h"
#include "CalypsoCardAdapter.h"

/* Keyple Core Util */
#include "LoggerFactory.h"

namespace keyple {
namespace card {
namespace calypso {

using namespace calypsonet::terminal::calypso;
using namespace calypsonet::terminal::calypso::sam;
using namespace calypsonet::terminal::calypso::transaction;
using namespace calypsonet::terminal::card;
using namespace keyple::core::util::cpp;

/**
 * (package-private)<br>
 * The SamCommandProcessor class is dedicated to the management of commands sent to the SAM.
 *
 * <p>In particular, it manages the cryptographic computations related to the secure session (digest
 * computation).
 *
 * <p>It also will integrate the SAM commands used for Stored Value and PIN/key management. In
 * session, these commands need to be carefully synchronized with the digest calculation.
 *
 * @since 2.0.0
 */
class SamCommandProcessor {
public:
    /**
     * Constructor
     *
     * @param calypsoCard The initial card data provided by the selection process.
     * @param cardSecuritySetting the security settings from the application layer.
     * @since 2.0.0
     */
    SamCommandProcessor(const std::shared_ptr<CalypsoCard> calypsoCard,
                        const std::shared_ptr<CardSecuritySetting> cardSecuritySetting);

    /**
     * Gets the terminal challenge
     *
     * <p>Performs key diversification if necessary by sending the SAM Select Diversifier command
     * prior to the Get Challenge command. The diversification flag is set to avoid further
     * unnecessary diversification operations.
     *
     * <p>If the key diversification is already done, the Select Diversifier command is omitted.
     *
     * <p>The length of the challenge varies from one card product type to another. This information
     * can be found in the CardResource class field.
     *
     * @return the terminal challenge as an array of bytes
     * @throw CalypsoSamCommandException if the SAM has responded with an error status
     * @throw ReaderBrokenCommunicationException if the communication with the SAM reader has failed.
     * @throw CardBrokenCommunicationException if the communication with the SAM has failed.
     * @throw DesynchronizedExchangesException if the APDU SAM exchanges are out of sync
     * @since 2.0.0
     */
    const std::vector<uint8_t> getSessionTerminalChallenge();

    /**
     * (package-private)<br>
     * Gets the KVC to use according to the provided write access and the card's KVC.
     *
     * @param writeAccessLevel The write access level.
     * @param kvc The card KVC value.
     * @return Null if the card did not provide a KVC value and if there's no default KVC value.
     * @since 2.0.0
     */
    const std::shared_ptr<uint8_t> computeKvc(const WriteAccessLevel writeAccessLevel,
                                              const std::shared_ptr<uint8_t> kvc) const;

    /**
     * (package-private)<br>
     * Gets the KIF to use according to the provided write access level and KVC.
     *
     * @param writeAccessLevel The write access level.
     * @param kif The card KIF value.
     * @param kvc The previously computed KVC value.
     * @return Null if the card did not provide a KIF value and if there's no default KIF value.
     * @since 2.0.0
     */
    const std::shared_ptr<uint8_t> computeKif(const WriteAccessLevel writeAccessLevel,
                                              const std::shared_ptr<uint8_t> kif,
                                              const std::shared_ptr<uint8_t> kvc);

    /**
     * Initializes the digest computation process
     *
     * <p>Resets the digest data cache, then fills a first packet with the provided data (from open
     * secure session).
     *
     * <p>Keeps the session parameters, sets the KIF if not defined
     *
     * <p>Note: there is no communication with the SAM here.
     *
     * @param sessionEncryption true if the session is encrypted.
     * @param verificationMode true if the verification mode is active.
     * @param kif the KIF.
     * @param kvc the KVC.
     * @param digestData a first packet of data to digest.
     * @since 2.0.0
     */
    void initializeDigester(const bool sessionEncryption,
                            const bool verificationMode,
                            const uint8_t kif,
                            const uint8_t kvc,
                            const std::vector<uint8_t>& digestData);

    /**
     * Appends a list full card exchange (request and response) to the digest data cache.<br>
     * The startIndex argument makes it possible not to include the beginning of the list when
     * necessary.
     *
     * @param requests card request list.
     * @param responses card response list.
     * @param startIndex starting point in the list.
     * @since 2.0.0
     */
    void pushCardExchangedData(const std::vector<std::shared_ptr<ApduRequestSpi>>& requests,
                               const std::vector<std::shared_ptr<ApduResponseApi>>& responses,
                               const int startIndex);

    // /**
    //  * Gets the terminal signature from the SAM
    //  *
    //  * <p>All remaining data in the digest cache is sent to the SAM and the Digest Close command is
    //  * executed.
    //  *
    //  * @return the terminal signature
    //  * @throws CalypsoSamCommandException if the SAM has responded with an error status
    //  * @throws ReaderBrokenCommunicationException if the communication with the SAM reader has failed.
    //  * @throws CardBrokenCommunicationException if the communication with the SAM has failed.
    //  * @throws DesynchronizedExchangesException if the APDU SAM exchanges are out of sync
    //  * @since 2.0.0
    //  */
    // byte[] getTerminalSignature()
    //     throws CalypsoSamCommandException, CardBrokenCommunicationException,
    //         ReaderBrokenCommunicationException {

    //     // All remaining SAM digest operations will now run at once.
    //     // Get the SAM Digest request including Digest Close from the cache manager
    //     List<AbstractSamCommand> samCommands = getPendingSamCommands(true);

    //     CardRequestSpi samCardRequest = new CardRequestAdapter(getApduRequests(samCommands), false);

    //     // Transmit CardRequest and get CardResponse
    //     CardResponseApi samCardResponse;

    //     try {
    //     samCardResponse = samReader.transmitCardRequest(samCardRequest, ChannelControl.KEEP_OPEN);
    //     } catch (UnexpectedStatusWordException e) {
    //     throw new IllegalStateException(UNEXPECTED_EXCEPTION, e);
    //     }

    //     List<ApduResponseApi> samApduResponses = samCardResponse.getApduResponses();

    //     if (samApduResponses.size() != samCommands.size()) {
    //     throw new DesynchronizedExchangesException(
    //         "The number of commands/responses does not match: cmd="
    //             + samCommands.size()
    //             + ", resp="
    //             + samApduResponses.size());
    //     }

    //     // check all responses status
    //     for (int i = 0; i < samApduResponses.size(); i++) {
    //     samCommands.get(i).setApduResponse(samApduResponses.get(i)).checkStatus();
    //     }

    //     // Get Terminal Signature from the latest response
    //     CmdSamDigestClose cmdSamDigestClose =
    //         (CmdSamDigestClose)
    //             samCommands
    //                 .get(samCommands.size() - 1)
    //                 .setApduResponse(samApduResponses.get(samCommands.size() - 1));

    //     byte[] sessionTerminalSignature = cmdSamDigestClose.getSignature();

    //     if (logger.isDebugEnabled()) {
    //     logger.debug("SIGNATURE = {}", ByteArrayUtil.toHex(sessionTerminalSignature));
    //     }

    //     return sessionTerminalSignature;
    // }

    // /**
    //  * Authenticates the signature part from the card
    //  *
    //  * <p>Executes the Digest Authenticate command with the card part of the signature.
    //  *
    //  * @param cardSignatureLo the card part of the signature.
    //  * @throws CalypsoSamCommandException if the SAM has responded with an error status
    //  * @throws ReaderBrokenCommunicationException if the communication with the SAM reader has failed.
    //  * @throws CardBrokenCommunicationException if the communication with the SAM has failed.
    //  * @throws DesynchronizedExchangesException if the APDU SAM exchanges are out of sync
    //  * @since 2.0.0
    //  */
    // void authenticateCardSignature(byte[] cardSignatureLo)
    //     throws CalypsoSamCommandException, CardBrokenCommunicationException,
    //         ReaderBrokenCommunicationException {
    //     // Check the card signature part with the SAM
    //     // Build and send SAM Digest Authenticate command
    //     CmdSamDigestAuthenticate cmdSamDigestAuthenticate =
    //         new CmdSamDigestAuthenticate(samProductType, cardSignatureLo);

    //     List<ApduRequestSpi> samApduRequests = new ArrayList<ApduRequestSpi>();
    //     samApduRequests.add(cmdSamDigestAuthenticate.getApduRequest());

    //     CardRequestSpi samCardRequest = new CardRequestAdapter(samApduRequests, false);

    //     CardResponseApi samCardResponse;
    //     try {
    //     samCardResponse = samReader.transmitCardRequest(samCardRequest, ChannelControl.KEEP_OPEN);
    //     } catch (UnexpectedStatusWordException e) {
    //     throw new IllegalStateException(UNEXPECTED_EXCEPTION, e);
    //     }

    //     // Get transaction result parsing the response
    //     List<ApduResponseApi> samApduResponses = samCardResponse.getApduResponses();

    //     if (samApduResponses == null || samApduResponses.isEmpty()) {
    //     throw new DesynchronizedExchangesException("No response to Digest Authenticate command.");
    //     }

    //     cmdSamDigestAuthenticate.setApduResponse(samApduResponses.get(0)).checkStatus();
    // }

    // /**
    //  * Create an ApduRequestAdapter List from a AbstractSamCommand List.
    //  *
    //  * @param samCommands a list of SAM commands.
    //  * @return the ApduRequestAdapter list
    //  * @since 2.0.0
    //  */
    // private List<ApduRequestSpi> getApduRequests(List<AbstractSamCommand> samCommands) {
    //     List<ApduRequestSpi> apduRequests = new ArrayList<ApduRequestSpi>();
    //     if (samCommands != null) {
    //     for (AbstractSamCommand samCommand : samCommands) {
    //         apduRequests.add(samCommand.getApduRequest());
    //     }
    //     }
    //     return apduRequests;
    // }

    // /**
    //  * (package-private)<br>
    //  * Compute the encrypted key data for the "Change Key" command.
    //  *
    //  * @param poChallenge The challenge from the card.
    //  * @param cipheringKif The KIF of the key used for encryption.
    //  * @param cipheringKvc The KVC of the key used for encryption.
    //  * @param sourceKif The KIF of the key to encrypt.
    //  * @param sourceKvc The KVC of the key to encrypt.
    //  * @return An array of 32 bytes containing the encrypted key.
    //  * @throws CalypsoSamCommandException if the SAM has responded with an error status
    //  * @throws ReaderBrokenCommunicationException if the communication with the SAM reader has failed.
    //  * @throws CardBrokenCommunicationException if the communication with the SAM has failed.
    //  * @since 2.1.0
    //  */
    // byte[] getEncryptedKey(
    //     byte[] poChallenge, byte cipheringKif, byte cipheringKvc, byte sourceKif, byte sourceKvc)
    //     throws CalypsoSamCommandException, CardBrokenCommunicationException,
    //         ReaderBrokenCommunicationException {
    //     List<AbstractSamCommand> samCommands = new ArrayList<AbstractSamCommand>();

    //     if (!isDiversificationDone) {
    //     // Build the SAM Select Diversifier command to provide the SAM with the card S/N
    //     // CL-SAM-CSN.1
    //     samCommands.add(
    //         new CmdSamSelectDiversifier(samProductType, calypsoCard.getCalypsoSerialNumberFull()));
    //     isDiversificationDone = true;
    //     }

    //     samCommands.add(new CmdSamGiveRandom(samProductType, poChallenge));

    //     int cardGenerateKeyCmdIndex = samCommands.size();

    //     CmdSamCardGenerateKey cmdSamCardGenerateKey =
    //         new CmdSamCardGenerateKey(samProductType, cipheringKif, cipheringKvc, sourceKif, sourceKvc);

    //     samCommands.add(cmdSamCardGenerateKey);

    //     // build a SAM CardRequest
    //     CardRequestSpi samCardRequest = new CardRequestAdapter(getApduRequests(samCommands), false);

    //     // execute the command
    //     CardResponseApi samCardResponse;
    //     try {
    //     samCardResponse = samReader.transmitCardRequest(samCardRequest, ChannelControl.KEEP_OPEN);
    //     } catch (UnexpectedStatusWordException e) {
    //     throw new IllegalStateException(UNEXPECTED_EXCEPTION, e);
    //     }

    //     ApduResponseApi cmdSamCardGenerateKeyResponse =
    //         samCardResponse.getApduResponses().get(cardGenerateKeyCmdIndex);

    //     // check execution status
    //     cmdSamCardGenerateKey.setApduResponse(cmdSamCardGenerateKeyResponse).checkStatus();

    //     return cmdSamCardGenerateKey.getCipheredData();
    // }

    // /**
    //  * (package-private)<br>
    //  * Compute the PIN ciphered data for the encrypted PIN verification or PIN update commands
    //  *
    //  * @param poChallenge the challenge from the card.
    //  * @param currentPin the current PIN value.
    //  * @param newPin the new PIN value (set to null if the operation is a PIN presentation).
    //  * @return the PIN ciphered data
    //  * @throws CalypsoSamCommandException if the SAM has responded with an error status
    //  * @throws ReaderBrokenCommunicationException if the communication with the SAM reader has failed.
    //  * @throws CardBrokenCommunicationException if the communication with the SAM has failed.
    //  * @since 2.0.0
    //  */
    // byte[] getCipheredPinData(byte[] poChallenge, byte[] currentPin, byte[] newPin)
    //     throws CalypsoSamCommandException, CardBrokenCommunicationException,
    //         ReaderBrokenCommunicationException {
    //     List<AbstractSamCommand> samCommands = new ArrayList<AbstractSamCommand>();
    //     byte pinCipheringKif;
    //     byte pinCipheringKvc;

    //     if (kif != 0) {
    //     // the current work key has been set (a secure session is open)
    //     pinCipheringKif = kif;
    //     pinCipheringKvc = kvc;
    //     } else {
    //     // no current work key is available (outside secure session)
    //     if (newPin == null) {
    //         // PIN verification
    //         if (((CardSecuritySettingAdapter) cardSecuritySettings).getPinVerificationCipheringKif()
    //                 == null
    //             || ((CardSecuritySettingAdapter) cardSecuritySettings).getPinVerificationCipheringKvc()
    //                 == null) {
    //         throw new IllegalStateException(
    //             "No KIF or KVC defined for the PIN verification ciphering key");
    //         }
    //         pinCipheringKif =
    //             ((CardSecuritySettingAdapter) cardSecuritySettings).getPinVerificationCipheringKif();
    //         pinCipheringKvc =
    //             ((CardSecuritySettingAdapter) cardSecuritySettings).getPinVerificationCipheringKvc();
    //     } else {
    //         // PIN modification
    //         if (((CardSecuritySettingAdapter) cardSecuritySettings).getPinModificationCipheringKif()
    //                 == null
    //             || ((CardSecuritySettingAdapter) cardSecuritySettings).getPinModificationCipheringKvc()
    //                 == null) {
    //         throw new IllegalStateException(
    //             "No KIF or KVC defined for the PIN modification ciphering key");
    //         }
    //         pinCipheringKif =
    //             ((CardSecuritySettingAdapter) cardSecuritySettings).getPinModificationCipheringKif();
    //         pinCipheringKvc =
    //             ((CardSecuritySettingAdapter) cardSecuritySettings).getPinModificationCipheringKvc();
    //     }
    //     }

    //     if (!isDiversificationDone) {
    //     // Build the SAM Select Diversifier command to provide the SAM with the card S/N
    //     // CL-SAM-CSN.1
    //     samCommands.add(
    //         new CmdSamSelectDiversifier(samProductType, calypsoCard.getCalypsoSerialNumberFull()));
    //     isDiversificationDone = true;
    //     }

    //     if (isDigesterInitialized) {
    //     /* Get the pending SAM ApduRequestAdapter and add it to the current ApduRequestAdapter list */
    //     samCommands.addAll(getPendingSamCommands(false));
    //     }

    //     samCommands.add(new CmdSamGiveRandom(samProductType, poChallenge));

    //     int cardCipherPinCmdIndex = samCommands.size();

    //     CmdSamCardCipherPin cmdSamCardCipherPin =
    //         new CmdSamCardCipherPin(
    //             samProductType, pinCipheringKif, pinCipheringKvc, currentPin, newPin);

    //     samCommands.add(cmdSamCardCipherPin);

    //     // build a SAM CardRequest
    //     CardRequestSpi samCardRequest = new CardRequestAdapter(getApduRequests(samCommands), false);

    //     // execute the command
    //     CardResponseApi samCardResponse;
    //     try {
    //     samCardResponse = samReader.transmitCardRequest(samCardRequest, ChannelControl.KEEP_OPEN);
    //     } catch (UnexpectedStatusWordException e) {
    //     throw new IllegalStateException(UNEXPECTED_EXCEPTION, e);
    //     }

    //     ApduResponseApi cardCipherPinResponse =
    //         samCardResponse.getApduResponses().get(cardCipherPinCmdIndex);

    //     // check execution status
    //     cmdSamCardCipherPin.setApduResponse(cardCipherPinResponse).checkStatus();

    //     return cmdSamCardCipherPin.getCipheredData();
    // }

    // /**
    //  * Generic method to get the complementary data from SvPrepareLoad/Debit/Undebit commands
    //  *
    //  * <p>Executes the SV Prepare SAM command to prepare the data needed to complete the card SV
    //  * command.
    //  *
    //  * <p>This data comprises:
    //  *
    //  * <ul>
    //  *   <li>The SAM identifier (4 bytes)
    //  *   <li>The SAM challenge (3 bytes)
    //  *   <li>The SAM transaction number (3 bytes)
    //  *   <li>The SAM part of the SV signature (5 or 10 bytes depending on card mode)
    //  * </ul>
    //  *
    //  * @param cmdSamSvPrepare the prepare command (can be prepareSvReload/Debit/Undebit).
    //  * @return a byte array containing the complementary data
    //  * @throws CalypsoSamCommandException if the SAM has responded with an error status
    //  * @throws ReaderBrokenCommunicationException if the communication with the SAM reader has failed.
    //  * @throws CardBrokenCommunicationException if the communication with the SAM has failed.
    //  * @since 2.0.0
    //  */
    // private byte[] getSvComplementaryData(AbstractSamCommand cmdSamSvPrepare)
    //     throws CalypsoSamCommandException, CardBrokenCommunicationException,
    //         ReaderBrokenCommunicationException {

    //     List<AbstractSamCommand> samCommands = new ArrayList<AbstractSamCommand>();

    //     if (!isDiversificationDone) {
    //     /* Build the SAM Select Diversifier command to provide the SAM with the card S/N */
    //     // CL-SAM-CSN.1
    //     samCommands.add(
    //         new CmdSamSelectDiversifier(samProductType, calypsoCard.getCalypsoSerialNumberFull()));
    //     isDiversificationDone = true;
    //     }

    //     if (isDigesterInitialized) {
    //     /* Get the pending SAM ApduRequestAdapter and add it to the current ApduRequestAdapter list */
    //     samCommands.addAll(getPendingSamCommands(false));
    //     }

    //     int svPrepareOperationCmdIndex = samCommands.size();

    //     samCommands.add(cmdSamSvPrepare);

    //     // build a SAM CardRequest
    //     CardRequestSpi samCardRequest = new CardRequestAdapter(getApduRequests(samCommands), false);

    //     // execute the command
    //     CardResponseApi samCardResponse;
    //     try {
    //     samCardResponse = samReader.transmitCardRequest(samCardRequest, ChannelControl.KEEP_OPEN);
    //     } catch (UnexpectedStatusWordException e) {
    //     throw new IllegalStateException(UNEXPECTED_EXCEPTION, e);
    //     }

    //     ApduResponseApi svPrepareResponse =
    //         samCardResponse.getApduResponses().get(svPrepareOperationCmdIndex);

    //     // check execution status
    //     cmdSamSvPrepare.setApduResponse(svPrepareResponse).checkStatus();

    //     byte[] prepareOperationData = cmdSamSvPrepare.getApduResponse().getDataOut();

    //     byte[] operationComplementaryData =
    //         new byte[samSerialNumber.length + prepareOperationData.length];

    //     System.arraycopy(samSerialNumber, 0, operationComplementaryData, 0, samSerialNumber.length);
    //     System.arraycopy(
    //         prepareOperationData,
    //         0,
    //         operationComplementaryData,
    //         samSerialNumber.length,
    //         prepareOperationData.length);

    //     return operationComplementaryData;
    // }

    // /**
    //  * Computes the cryptographic data required for the SvReload command.
    //  *
    //  * <p>Use the data from the SvGet command and the partial data from the SvReload command for this
    //  * purpose.
    //  *
    //  * <p>The returned data will be used to finalize the card SvReload command.
    //  *
    //  * @param cmdCardSvReload the SvDebit command providing the SvReload partial data.
    //  * @param svGetHeader the SV Get command header.
    //  * @param svGetData the SV Get command response data.
    //  * @return the complementary security data to finalize the SvReload card command (sam ID + SV
    //  *     prepare load output)
    //  * @throws CalypsoSamCommandException if the SAM has responded with an error status
    //  * @throws ReaderBrokenCommunicationException if the communication with the SAM reader has failed.
    //  * @throws CardBrokenCommunicationException if the communication with the SAM has failed.
    //  * @since 2.0.0
    //  */
    // byte[] getSvReloadComplementaryData(
    //     CmdCardSvReload cmdCardSvReload, byte[] svGetHeader, byte[] svGetData)
    //     throws CalypsoSamCommandException, ReaderBrokenCommunicationException,
    //         CardBrokenCommunicationException {
    //     // get the complementary data from the SAM
    //     CmdSamSvPrepareLoad cmdSamSvPrepareLoad =
    //         new CmdSamSvPrepareLoad(
    //             samProductType, svGetHeader, svGetData, cmdCardSvReload.getSvReloadData());

    //     return getSvComplementaryData(cmdSamSvPrepareLoad);
    // }

    // /**
    //  * Computes the cryptographic data required for the SvDebit command.
    //  *
    //  * <p>Use the data from the SvGet command and the partial data from the SvDebit command for this
    //  * purpose.
    //  *
    //  * <p>The returned data will be used to finalize the card SvDebit command.
    //  *
    //  * @param svGetHeader the SV Get command header.
    //  * @param svGetData the SV Get command response data.
    //  * @return the complementary security data to finalize the SvDebit card command (sam ID + SV
    //  *     prepare load output)
    //  * @throws CalypsoSamCommandException if the SAM has responded with an error status
    //  * @throws ReaderBrokenCommunicationException if the communication with the SAM reader has failed.
    //  * @throws CardBrokenCommunicationException if the communication with the SAM has failed.
    //  * @since 2.0.0
    //  */
    // byte[] getSvDebitComplementaryData(
    //     CmdCardSvDebit cmdCardSvDebit, byte[] svGetHeader, byte[] svGetData)
    //     throws CalypsoSamCommandException, ReaderBrokenCommunicationException,
    //         CardBrokenCommunicationException {
    //     // get the complementary data from the SAM
    //     CmdSamSvPrepareDebit cmdSamSvPrepareDebit =
    //         new CmdSamSvPrepareDebit(
    //             samProductType, svGetHeader, svGetData, cmdCardSvDebit.getSvDebitData());

    //     return getSvComplementaryData(cmdSamSvPrepareDebit);
    // }

    // /**
    //  * Computes the cryptographic data required for the SvUndebit command.
    //  *
    //  * <p>Use the data from the SvGet command and the partial data from the SvUndebit command for this
    //  * purpose.
    //  *
    //  * <p>The returned data will be used to finalize the card SvUndebit command.
    //  *
    //  * @param svGetHeader the SV Get command header.
    //  * @param svGetData the SV Get command response data.
    //  * @return the complementary security data to finalize the SvUndebit card command (sam ID + SV
    //  *     prepare load output)
    //  * @throws CalypsoSamCommandException if the SAM has responded with an error status
    //  * @throws ReaderBrokenCommunicationException if the communication with the SAM reader has failed.
    //  * @throws CardBrokenCommunicationException if the communication with the SAM has failed.
    //  * @since 2.0.0
    //  */
    // public byte[] getSvUndebitComplementaryData(
    //     CmdCardSvUndebit cmdCardSvUndebit, byte[] svGetHeader, byte[] svGetData)
    //     throws CalypsoSamCommandException, ReaderBrokenCommunicationException,
    //         CardBrokenCommunicationException {
    //     // get the complementary data from the SAM
    //     CmdSamSvPrepareUndebit cmdSamSvPrepareUndebit =
    //         new CmdSamSvPrepareUndebit(
    //             samProductType, svGetHeader, svGetData, cmdCardSvUndebit.getSvUndebitData());

    //     return getSvComplementaryData(cmdSamSvPrepareUndebit);
    // }

    // /**
    //  * Checks the status of the last SV operation
    //  *
    //  * <p>The card signature is compared by the SAM with the one it has computed on its side.
    //  *
    //  * @param svOperationResponseData the data of the SV operation performed.
    //  * @throws CalypsoSamCommandException if the SAM has responded with an error status
    //  * @throws ReaderBrokenCommunicationException if the communication with the SAM reader has failed.
    //  * @throws CardBrokenCommunicationException if the communication with the SAM has failed.
    //  * @since 2.0.0
    //  */
    // void checkSvStatus(byte[] svOperationResponseData)
    //     throws CalypsoSamCommandException, CardBrokenCommunicationException,
    //         ReaderBrokenCommunicationException {
    //     List<AbstractSamCommand> samCommands = new ArrayList<AbstractSamCommand>();

    //     CmdSamSvCheck cmdSamSvCheck = new CmdSamSvCheck(samProductType, svOperationResponseData);
    //     samCommands.add(cmdSamSvCheck);

    //     // build a SAM CardRequest
    //     CardRequestSpi samCardRequest = new CardRequestAdapter(getApduRequests(samCommands), false);

    //     // execute the command
    //     CardResponseApi samCardResponse;
    //     try {
    //     samCardResponse = samReader.transmitCardRequest(samCardRequest, ChannelControl.KEEP_OPEN);
    //     } catch (UnexpectedStatusWordException e) {
    //     throw new IllegalStateException(UNEXPECTED_EXCEPTION, e);
    //     }

    //     ApduResponseApi svCheckResponse = samCardResponse.getApduResponses().get(0);

    //     // check execution status
    //     cmdSamSvCheck.setApduResponse(svCheckResponse).checkStatus();
    // }

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger = LoggerFactory::getLogger(typeid(SamCommandProcessor));

    /**
     *
     */
    static const uint8_t KIF_UNDEFINED;
    static const uint8_t CHALLENGE_LENGTH_REV_INF_32;
    static const uint8_t CHALLENGE_LENGTH_REV32;
    static const uint8_t SIGNATURE_LENGTH_REV_INF_32;
    static const uint8_t SIGNATURE_LENGTH_REV32;
    static const std::string UNEXPECTED_EXCEPTION;

    /**
     */
    std::shared_ptr<ProxyReaderApi> mSamReader;

    /**
     *
     */
    const std::shared_ptr<CardSecuritySetting> mCardSecuritySettings;

    /**
     *
     */
    static std::vector<std::vector<uint8_t>> mCardDigestDataCache;

    /**
     *
     */
    const std::shared_ptr<CalypsoCardAdapter> mCalypsoCard;

    /**
     *
     */
    std::vector<uint8_t> mSamSerialNumber;

    /**
     *
     */
    CalypsoSam::ProductType mSamProductType;

    /**
     *
     */
    bool mSessionEncryption;

    /**
     *
     */
    bool mVerificationMode;

    /**
     *
     */
    uint8_t mKif;

    /**
     *
     */
    uint8_t mKvc;

    /**
     *
     */
    bool mIsDiversificationDone;

    /**
     *
     */
    bool mIsDigestInitDone;

    /**
     *
     */
    bool mIsDigesterInitialized;

     /**
     * Appends a full card exchange (request and response) to the digest data cache.
     *
     * @param request card request.
     * @param response card response.
     * @since 2.0.0
     */
    void pushCardExchangedData(const std::shared_ptr<ApduRequestSpi> request,
                               const std::shared_ptr<ApduResponseApi> response);

    /**
     * Gets a single SAM request for all prepared SAM commands.
     *
     * <p>Builds all pending SAM commands related to the digest calculation process of a secure
     * session
     *
     * <ul>
     *   <li>Starts with a Digest Init command if not already done,
     *   <li>Adds as many Digest Update commands as there are packages in the cache,
     *   <li>Appends a Digest Close command if the addDigestClose flag is set to true.
     * </ul>
     *
     * @param addDigestClose indicates whether to add the Digest Close command.
     * @return a list of commands to send to the SAM
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<AbstractSamCommand>> getPendingSamCommands(
        const bool addDigestClose);
};

}
}
}
