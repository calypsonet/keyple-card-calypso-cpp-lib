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

#include "CalypsoCardUtilAdapter.h"

/* Keyple Card Calypso */
#include "CalypsoCardCommand.h"
#include "CalypsoCardConstant.h"
#include "CardDataAccessException.h"
#include "CmdCardGetDataFci.h"
#include "CmdCardGetDataFcp.h"
#include "CmdCardSearchRecordMultiple.h"
#include "CmdCardSelectFile.h"
#include "DirectoryHeaderAdapter.h"

/* Keyple Core Util */
#include "Arrays.h"
#include "IllegalStateException.h"
#include "System.h"

namespace keyple {
namespace card {
namespace calypso {

using namespace keyple::core::util::cpp;
using namespace keyple::core::util::cpp::exception;

CalypsoCardUtilAdapter::CalypsoCardUtilAdapter() {}

void CalypsoCardUtilAdapter::updateCalypsoCard(
    std::shared_ptr<CalypsoCardAdapter> calypsoCard,
    const std::shared_ptr<AbstractCardCommand> command,
    const std::shared_ptr<ApduResponseApi> apduResponse,
    const bool isSessionOpen)
{
    if (command->getCommandRef() == CalypsoCardCommand::READ_RECORDS) {
        updateCalypsoCardReadRecords(calypsoCard,
                                     std::dynamic_pointer_cast<CmdCardReadRecords>(command),
                                     apduResponse,
                                     isSessionOpen);
    } else if (command->getCommandRef() ==  CalypsoCardCommand::GET_DATA) {
        if (std::dynamic_pointer_cast<CmdCardGetDataFci>(command)) {
            calypsoCard->initializeWithFci(apduResponse);
        } else if (std::dynamic_pointer_cast<CmdCardGetDataFcp>(command)) {
            updateCalypsoCardWithFcp(calypsoCard, command, apduResponse);
        } else if (std::dynamic_pointer_cast<CmdCardGetDataEfList>(command)) {
            updateCalypsoCardWithEfList(calypsoCard,
                                        std::dynamic_pointer_cast<CmdCardGetDataEfList>(command),
                                        apduResponse);
        } else if (std::dynamic_pointer_cast<CmdCardGetDataTraceabilityInformation>(command)) {
            updateCalypsoCardWithTraceabilityInformation(
                calypsoCard,
                std::dynamic_pointer_cast<CmdCardGetDataTraceabilityInformation>(command),
                apduResponse);
        } else {
            throw IllegalStateException("Unknown GET DATA command reference.");
        }
    } else if (command->getCommandRef() ==  CalypsoCardCommand::SEARCH_RECORD_MULTIPLE) {
        updateCalypsoCardSearchRecordMultiple(
            calypsoCard,
            std::dynamic_pointer_cast<CmdCardSearchRecordMultiple>(command),
            apduResponse,
            isSessionOpen);
    } else if (command->getCommandRef() ==  CalypsoCardCommand::READ_RECORD_MULTIPLE) {
        updateCalypsoCardReadRecordMultiple(
            calypsoCard,
            std::dynamic_pointer_cast<CmdCardReadRecordMultiple>(command),
            apduResponse,
            isSessionOpen);
    } else if (command->getCommandRef() ==  CalypsoCardCommand::SELECT_FILE) {
        updateCalypsoCardWithFcp(calypsoCard, command, apduResponse);
    } else if (command->getCommandRef() ==  CalypsoCardCommand::UPDATE_RECORD) {
        updateCalypsoCardUpdateRecord(calypsoCard,
                                     std::dynamic_pointer_cast<CmdCardUpdateRecord>(command),
                                     apduResponse);
    } else if (command->getCommandRef() ==  CalypsoCardCommand::WRITE_RECORD) {
        updateCalypsoCardWriteRecord(
            calypsoCard,
            std::dynamic_pointer_cast<CmdCardWriteRecord>(command),
            apduResponse);
    } else if (command->getCommandRef() ==  CalypsoCardCommand::APPEND_RECORD) {
        updateCalypsoCardAppendRecord(calypsoCard,
                                      std::dynamic_pointer_cast<CmdCardAppendRecord>(command),
                                      apduResponse);
    } else if (command->getCommandRef() ==  CalypsoCardCommand::INCREASE ||
               command->getCommandRef() ==  CalypsoCardCommand::DECREASE) {
        updateCalypsoCardIncreaseOrDecrease(
            calypsoCard,
            std::dynamic_pointer_cast<CmdCardIncreaseOrDecrease>(command),
            apduResponse);
    } else if (command->getCommandRef() ==  CalypsoCardCommand::INCREASE_MULTIPLE ||
               command->getCommandRef() ==  CalypsoCardCommand::DECREASE_MULTIPLE) {
        updateCalypsoCardIncreaseOrDecreaseMultiple(
            calypsoCard,
            std::dynamic_pointer_cast<CmdCardIncreaseOrDecreaseMultiple>(command),
            apduResponse);
    } else if (command->getCommandRef() ==  CalypsoCardCommand::OPEN_SESSION) {
        updateCalypsoCardOpenSession(calypsoCard,
                                     std::dynamic_pointer_cast<CmdCardOpenSession>(command),
                                     apduResponse);
    // case CLOSE_SESSION:
    //     updateCalypsoCardCloseSession((CmdCardCloseSession) command, apduResponse);
    //     break;
    // case READ_BINARY:
    //     updateCalypsoCardReadBinary(
    //         calypsoCard, (CmdCardReadBinary) command, apduResponse, isSessionOpen);
    //     break;
    // case UPDATE_BINARY:
    //     updateCalypsoCardUpdateBinary(
    //         calypsoCard, (CmdCardUpdateOrWriteBinary) command, apduResponse);
    //     break;
    // case WRITE_BINARY:
    //     updateCalypsoCardWriteBinary(
    //         calypsoCard, (CmdCardUpdateOrWriteBinary) command, apduResponse);
    //     break;
    // case GET_CHALLENGE:
    //     updateCalypsoCardGetChallenge(calypsoCard, (CmdCardGetChallenge) command, apduResponse);
    //     break;
    // case VERIFY_PIN:
    //     updateCalypsoVerifyPin(calypsoCard, (CmdCardVerifyPin) command, apduResponse);
    //     break;
    // case SV_GET:
    //     updateCalypsoCardSvGet(calypsoCard, (CmdCardSvGet) command, apduResponse);
    //     break;
    // case SV_RELOAD:
    // case SV_DEBIT:
    // case SV_UNDEBIT:
    //     updateCalypsoCardSvOperation(calypsoCard, command, apduResponse);
    //     break;
    // case INVALIDATE:
    // case REHABILITATE:
    //     updateCalypsoInvalidateRehabilitate(command, apduResponse);
    //     break;
    // case CHANGE_PIN:
    //     updateCalypsoChangePin((CmdCardChangePin) command, apduResponse);
    //     break;
    // case CHANGE_KEY:
    //     updateCalypsoChangeKey((CmdCardChangeKey) command, apduResponse);
    //     break;
    } else {
        throw IllegalStateException("Unknown command reference.");
    }
}

void CalypsoCardUtilAdapter::updateCalypsoCard(
    std::shared_ptr<CalypsoCardAdapter> calypsoCard,
    const std::vector<std::shared_ptr<AbstractCardCommand>>& commands,
    const std::vector<std::shared_ptr<ApduResponseApi>>& apduResponses,
    const bool isSessionOpen)
{

    auto responseIterator = apduResponses.begin();

    if (!commands.empty()) {
        for (const auto&  command : commands) {
            const std::shared_ptr<ApduResponseApi> apduResponse = *responseIterator++;
            updateCalypsoCard(calypsoCard, command, apduResponse, isSessionOpen);
        }
    }
}

void CalypsoCardUtilAdapter::updateCalypsoCardOpenSession(
    std::shared_ptr<CalypsoCardAdapter> calypsoCard,
    std::shared_ptr<CmdCardOpenSession> cmdCardOpenSession,
    const std::shared_ptr<ApduResponseApi> apduResponse)
{
    cmdCardOpenSession->setApduResponse(apduResponse);

    /* CL-CSS-INFORAT.1 */
    calypsoCard->setDfRatified(cmdCardOpenSession->wasRatified());

    const std::vector<uint8_t>& recordDataRead = cmdCardOpenSession->getRecordDataRead();

    if (recordDataRead.size() > 0) {
        calypsoCard->setContent(cmdCardOpenSession->getSfi(),
                                cmdCardOpenSession->getRecordNumber(),
                                recordDataRead);
    }
}

void CalypsoCardUtilAdapter::updateCalypsoCardReadRecords(
    const std::shared_ptr<CalypsoCardAdapter> calypsoCard,
    const std::shared_ptr<CmdCardReadRecords> cmdCardReadRecords,
    const std::shared_ptr<ApduResponseApi> apduResponse,
    const bool isSessionOpen)
{
    cmdCardReadRecords->setApduResponse(apduResponse);
    checkResponseStatusForStrictAndBestEffortMode(cmdCardReadRecords, isSessionOpen);

    /* Iterate over read records to fill the CalypsoCard */
    for (const auto& entry : cmdCardReadRecords->getRecords()) {
        calypsoCard->setContent(cmdCardReadRecords->getSfi(), entry.first, entry.second);
    }
}

void CalypsoCardUtilAdapter::checkResponseStatusForStrictAndBestEffortMode(
    const std::shared_ptr<AbstractCardCommand> command, const bool isSessionOpen)
{
    if (isSessionOpen) {
        command->checkStatus();
    } else {
        try {
            command->checkStatus();
        } catch (const CardDataAccessException& e) {
            /*
             * Best effort mode, do not throw exception for "file not found" and "record not found"
             * errors.
             */
            if (command->getApduResponse()->getStatusWord() != 0x6A82 &&
                command->getApduResponse()->getStatusWord() != 0x6A83) {
                throw e;
            }
        }
    }
}

void CalypsoCardUtilAdapter::updateCalypsoCardSearchRecordMultiple(
    std::shared_ptr<CalypsoCardAdapter> calypsoCard,
    std::shared_ptr<CmdCardSearchRecordMultiple> cmdCardSearchRecordMultiple,
    const std::shared_ptr<ApduResponseApi> apduResponse,
    const bool isSessionOpen)
{

    cmdCardSearchRecordMultiple->setApduResponse(apduResponse);
    checkResponseStatusForStrictAndBestEffortMode(cmdCardSearchRecordMultiple, isSessionOpen);

    if (cmdCardSearchRecordMultiple->getFirstMatchingRecordContent().size() > 0) {
        calypsoCard->setContent(
            cmdCardSearchRecordMultiple->getSearchCommandData()->getSfi(),
            cmdCardSearchRecordMultiple->getSearchCommandData()->getMatchingRecordNumbers()[0],
            cmdCardSearchRecordMultiple->getFirstMatchingRecordContent());
    }
}

void CalypsoCardUtilAdapter::updateCalypsoCardReadRecordMultiple(
    std::shared_ptr<CalypsoCardAdapter> calypsoCard,
    std::shared_ptr<CmdCardReadRecordMultiple> cmdCardReadRecordMultiple,
    const std::shared_ptr<ApduResponseApi> apduResponse,
    const bool isSessionOpen)
{
    cmdCardReadRecordMultiple->setApduResponse(apduResponse);
    checkResponseStatusForStrictAndBestEffortMode(cmdCardReadRecordMultiple, isSessionOpen);

    for (const auto& entry : cmdCardReadRecordMultiple->getResults()) {
        calypsoCard->setContent(cmdCardReadRecordMultiple->getSfi(),
                                entry.first,
                                entry.second,
                                cmdCardReadRecordMultiple->getOffset());
    }
}

void CalypsoCardUtilAdapter::updateCalypsoCardWithFcp(
    std::shared_ptr<CalypsoCardAdapter> calypsoCard,
    std::shared_ptr<AbstractCardCommand> command,
    const std::shared_ptr<ApduResponseApi> apduResponse)
{

    command->setApduResponse(apduResponse).checkStatus();

    std::vector<uint8_t> proprietaryInformation;
    if (command->getCommandRef() == CalypsoCardCommand::SELECT_FILE) {
        proprietaryInformation =
            std::dynamic_pointer_cast<CmdCardSelectFile>(command)->getProprietaryInformation();
    } else {
        proprietaryInformation =
            std::dynamic_pointer_cast<CmdCardGetDataFcp>(command)->getProprietaryInformation();
    }

    const uint8_t sfi = proprietaryInformation[CalypsoCardConstant::SEL_SFI_OFFSET];
    const uint8_t fileType = proprietaryInformation[CalypsoCardConstant::SEL_TYPE_OFFSET];

    if (fileType == CalypsoCardConstant::FILE_TYPE_MF ||
        fileType == CalypsoCardConstant::FILE_TYPE_MF ||
        fileType == CalypsoCardConstant::FILE_TYPE_DF) {
        const auto directoryHeader = createDirectoryHeader(proprietaryInformation);
        calypsoCard->setDirectoryHeader(directoryHeader);
    } else if (fileType == CalypsoCardConstant::FILE_TYPE_EF) {
        auto fileHeader = createFileHeader(proprietaryInformation);
        calypsoCard->setFileHeader(sfi, fileHeader);
    } else {
        throw IllegalStateException("Unknown file type: " + std::to_string(fileType));
    }
}

void CalypsoCardUtilAdapter::updateCalypsoCardWithEfList(
    std::shared_ptr<CalypsoCardAdapter> calypsoCard,
    std::shared_ptr<CmdCardGetDataEfList> command,
    const std::shared_ptr<ApduResponseApi> apduResponse)
{

    command->setApduResponse(apduResponse).checkStatus();

    const std::map<const std::shared_ptr<FileHeaderAdapter>, const uint8_t> fileHeaderToSfiMap =
        command->getEfHeaders();

    for (const auto& entry : fileHeaderToSfiMap) {
        calypsoCard->setFileHeader(entry.second, entry.first);
    }
}

void CalypsoCardUtilAdapter::updateCalypsoCardWithTraceabilityInformation(
    std::shared_ptr<CalypsoCardAdapter> calypsoCard,
    std::shared_ptr<CmdCardGetDataTraceabilityInformation> command,
    const std::shared_ptr<ApduResponseApi> apduResponse)
{

    command->setApduResponse(apduResponse).checkStatus();

    calypsoCard->setTraceabilityInformation(apduResponse->getDataOut());
}

void CalypsoCardUtilAdapter::updateCalypsoCardUpdateRecord(
    std::shared_ptr<CalypsoCardAdapter> calypsoCard,
    std::shared_ptr<CmdCardUpdateRecord> cmdCardUpdateRecord,
    const std::shared_ptr<ApduResponseApi> apduResponse)
{
    cmdCardUpdateRecord->setApduResponse(apduResponse).checkStatus();

    calypsoCard->setContent(cmdCardUpdateRecord->getSfi(),
                            cmdCardUpdateRecord->getRecordNumber(),
                            cmdCardUpdateRecord->getData());
}

void CalypsoCardUtilAdapter::updateCalypsoCardWriteRecord(
    std::shared_ptr<CalypsoCardAdapter> calypsoCard,
    std::shared_ptr<CmdCardWriteRecord> cmdCardWriteRecord,
    const std::shared_ptr<ApduResponseApi> apduResponse)
{

    cmdCardWriteRecord->setApduResponse(apduResponse).checkStatus();

    calypsoCard->fillContent(cmdCardWriteRecord->getSfi(),
                             cmdCardWriteRecord->getRecordNumber(),
                             cmdCardWriteRecord->getData(),
                             0);
}

void CalypsoCardUtilAdapter::updateCalypsoCardAppendRecord(
    std::shared_ptr<CalypsoCardAdapter> calypsoCard,
    std::shared_ptr<CmdCardAppendRecord> cmdCardAppendRecord,
    const std::shared_ptr<ApduResponseApi> apduResponse)
{
    cmdCardAppendRecord->setApduResponse(apduResponse).checkStatus();

    calypsoCard->addCyclicContent(cmdCardAppendRecord->getSfi(), cmdCardAppendRecord->getData());
}

void CalypsoCardUtilAdapter::updateCalypsoCardIncreaseOrDecrease(
    std::shared_ptr<CalypsoCardAdapter> calypsoCard,
    std::shared_ptr<CmdCardIncreaseOrDecrease> cmdCardIncreaseOrDecrease,
    const std::shared_ptr<ApduResponseApi> apduResponse)
{

    cmdCardIncreaseOrDecrease->setApduResponse(apduResponse).checkStatus();

    calypsoCard->setCounter(cmdCardIncreaseOrDecrease->getSfi(),
                            cmdCardIncreaseOrDecrease->getCounterNumber(),
                            apduResponse->getDataOut());
}

void CalypsoCardUtilAdapter::updateCalypsoCardIncreaseOrDecreaseMultiple(
    std::shared_ptr<CalypsoCardAdapter> calypsoCard,
    std::shared_ptr<CmdCardIncreaseOrDecreaseMultiple> cmdCardIncreaseOrDecreaseMultiple,
    const std::shared_ptr<ApduResponseApi> apduResponse)
{

    cmdCardIncreaseOrDecreaseMultiple->setApduResponse(apduResponse).checkStatus();

    for (const auto& entry : cmdCardIncreaseOrDecreaseMultiple->getNewCounterValues()) {
        calypsoCard->setCounter(cmdCardIncreaseOrDecreaseMultiple->getSfi(),
                                entry.first,
                                entry.second);
    }
}

const std::shared_ptr<DirectoryHeader> CalypsoCardUtilAdapter::createDirectoryHeader(
    const std::vector<uint8_t>& proprietaryInformation)
{
    std::vector<uint8_t> accessConditions(CalypsoCardConstant::SEL_AC_LENGTH);
    System::arraycopy(proprietaryInformation,
                      CalypsoCardConstant::SEL_AC_OFFSET,
                      accessConditions,
                      0,
                      CalypsoCardConstant::SEL_AC_LENGTH);

    std::vector<uint8_t> keyIndexes(CalypsoCardConstant::SEL_NKEY_LENGTH);
    System::arraycopy(proprietaryInformation,
                      CalypsoCardConstant::SEL_NKEY_OFFSET,
                      keyIndexes,
                      0,
                      CalypsoCardConstant::SEL_NKEY_LENGTH);

    const uint8_t dfStatus = proprietaryInformation[CalypsoCardConstant::SEL_DF_STATUS_OFFSET];

    uint16_t lid = (((proprietaryInformation[CalypsoCardConstant::SEL_LID_OFFSET] << 8) & 0xff00) |
                     (proprietaryInformation[CalypsoCardConstant::SEL_LID_OFFSET + 1] & 0x00ff));

    return DirectoryHeaderAdapter::builder()
               ->lid(lid)
                .accessConditions(accessConditions)
                .keyIndexes(keyIndexes)
                .dfStatus(dfStatus)
                .kvc(WriteAccessLevel::PERSONALIZATION,
                     proprietaryInformation[CalypsoCardConstant::SEL_KVCS_OFFSET])
                .kvc(WriteAccessLevel::LOAD,
                     proprietaryInformation[CalypsoCardConstant::SEL_KVCS_OFFSET + 1])
                .kvc(WriteAccessLevel::DEBIT,
                     proprietaryInformation[CalypsoCardConstant::SEL_KVCS_OFFSET + 2])
                .kif(WriteAccessLevel::PERSONALIZATION,
                     proprietaryInformation[CalypsoCardConstant::SEL_KIFS_OFFSET])
                .kif(WriteAccessLevel::LOAD,
                     proprietaryInformation[CalypsoCardConstant::SEL_KIFS_OFFSET + 1])
                .kif(WriteAccessLevel::DEBIT,
                     proprietaryInformation[CalypsoCardConstant::SEL_KIFS_OFFSET + 2])
                .build();
}

ElementaryFile::Type CalypsoCardUtilAdapter::getEfTypeFromCardValue(const uint8_t efType)
{
    ElementaryFile::Type fileType;

    if (efType == CalypsoCardConstant::EF_TYPE_BINARY) {
        fileType = ElementaryFile::Type::BINARY;
    } else if (efType == CalypsoCardConstant::EF_TYPE_LINEAR) {
        fileType = ElementaryFile::Type::LINEAR;
    } else if (efType == CalypsoCardConstant::EF_TYPE_CYCLIC) {
        fileType = ElementaryFile::Type::CYCLIC;
    } else if (efType == CalypsoCardConstant::EF_TYPE_SIMULATED_COUNTERS) {
        fileType = ElementaryFile::Type::SIMULATED_COUNTERS;
    } else if (efType == CalypsoCardConstant::EF_TYPE_COUNTERS) {
        fileType = ElementaryFile::Type::COUNTERS;
    } else {
        throw IllegalStateException("Unknown EF Type: " + std::to_string(efType));
    }

    return fileType;
}

const std::shared_ptr<FileHeaderAdapter> CalypsoCardUtilAdapter::createFileHeader(
    const std::vector<uint8_t>& proprietaryInformation)
{
    const ElementaryFile::Type fileType =
        getEfTypeFromCardValue(proprietaryInformation[CalypsoCardConstant::SEL_EF_TYPE_OFFSET]);

    int recordSize;
    int recordsNumber;

    if (fileType == ElementaryFile::Type::BINARY) {
        recordSize =
            ((proprietaryInformation[CalypsoCardConstant::SEL_REC_SIZE_OFFSET] << 8) & 0x0000ff00) |
             (proprietaryInformation[CalypsoCardConstant::SEL_NUM_REC_OFFSET] & 0x000000ff);
        recordsNumber = 1;
    } else {
        recordSize = proprietaryInformation[CalypsoCardConstant::SEL_REC_SIZE_OFFSET];
        recordsNumber = proprietaryInformation[CalypsoCardConstant::SEL_NUM_REC_OFFSET];
    }

    std::vector<uint8_t> accessConditions(CalypsoCardConstant::SEL_AC_LENGTH);
    System::arraycopy(proprietaryInformation,
                      CalypsoCardConstant::SEL_AC_OFFSET,
                      accessConditions,
                      0,
                      CalypsoCardConstant::SEL_AC_LENGTH);

    std::vector<uint8_t> keyIndexes(CalypsoCardConstant::SEL_NKEY_LENGTH);
    System::arraycopy(proprietaryInformation,
                      CalypsoCardConstant::SEL_NKEY_OFFSET,
                      keyIndexes,
                      0,
                      CalypsoCardConstant::SEL_NKEY_LENGTH);

    const uint8_t dfStatus = proprietaryInformation[CalypsoCardConstant::SEL_DF_STATUS_OFFSET];

    const uint16_t sharedReference =
        ((proprietaryInformation[CalypsoCardConstant::SEL_DATA_REF_OFFSET] << 8) & 0xff00) |
        (proprietaryInformation[CalypsoCardConstant::SEL_DATA_REF_OFFSET + 1] & 0x00ff);

    const uint16_t lid =
             ((proprietaryInformation[CalypsoCardConstant::SEL_LID_OFFSET] << 8) & 0xff00) |
             (proprietaryInformation[CalypsoCardConstant::SEL_LID_OFFSET + 1] & 0x00ff);

    return FileHeaderAdapter::builder()->lid(lid)
                                        .recordsNumber(recordsNumber)
                                        .recordSize(recordSize)
                                        .type(fileType)
                                        .accessConditions(accessConditions)
                                        .keyIndexes(keyIndexes)
                                        .dfStatus(dfStatus)
                                        .sharedReference(sharedReference)
                                        .build();
}

}
}
}
