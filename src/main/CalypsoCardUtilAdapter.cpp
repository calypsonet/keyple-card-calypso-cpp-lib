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

/* Keyple Core Util */
#include "IllegalStateException.h"

namespace keyple {
namespace card {
namespace calypso {

using namespace keyple::core::util::cpp::exception;

CalypsoCardUtilAdapter::CalypsoCardUtilAdapter() {}

void CalypsoCardUtilAdapter::updateCalypsoCard(
    const std::shared_ptr<CalypsoCardAdapter> calypsoCard,
    const std::shared_ptr<AbstractCardCommand> command,
    const std::shared_ptr<ApduResponseApi> apduResponse,
    const bool isSessionOpen)
{
    switch (command->getCommandRef()) {
    case CalypsoCardCommand::READ_RECORDS:
        updateCalypsoCardReadRecords(calypsoCard,
                                     std::dynamic_pointer_cast<CmdCardReadRecords>(command),
                                     apduResponse,
                                     isSessionOpen);
        break;
    // case GET_DATA:
        // if (command instanceof CmdCardGetDataFci) {
    //     calypsoCard.initializeWithFci(apduResponse);
    //     } else if (command instanceof CmdCardGetDataFcp) {
    //     updateCalypsoCardWithFcp(calypsoCard, command, apduResponse);
    //     } else if (command instanceof CmdCardGetDataEfList) {
    //     updateCalypsoCardWithEfList(calypsoCard, (CmdCardGetDataEfList) command, apduResponse);
    //     } else if (command instanceof CmdCardGetDataTraceabilityInformation) {
    //     updateCalypsoCardWithTraceabilityInformation(
    //         calypsoCard, (CmdCardGetDataTraceabilityInformation) command, apduResponse);
    //     } else {
    //     throw new IllegalStateException("Unknown GET DATA command reference.");
    //     }
    //     break;
    // case SEARCH_RECORD_MULTIPLE:
    //     updateCalypsoCardSearchRecordMultiple(
    //         calypsoCard, (CmdCardSearchRecordMultiple) command, apduResponse, isSessionOpen);
    //     break;
    // case READ_RECORD_MULTIPLE:
    //     updateCalypsoCardReadRecordMultiple(
    //         calypsoCard, (CmdCardReadRecordMultiple) command, apduResponse, isSessionOpen);
    //     break;
    // case SELECT_FILE:
    //     updateCalypsoCardWithFcp(calypsoCard, command, apduResponse);
    //     break;
    // case UPDATE_RECORD:
    //     updateCalypsoCardUpdateRecord(calypsoCard, (CmdCardUpdateRecord) command, apduResponse);
    //     break;
    // case WRITE_RECORD:
    //     updateCalypsoCardWriteRecord(calypsoCard, (CmdCardWriteRecord) command, apduResponse);
    //     break;
    // case APPEND_RECORD:
    //     updateCalypsoCardAppendRecord(calypsoCard, (CmdCardAppendRecord) command, apduResponse);
    //     break;
    // case INCREASE:
    // case DECREASE:
    //     updateCalypsoCardIncreaseOrDecrease(
    //         calypsoCard, (CmdCardIncreaseOrDecrease) command, apduResponse);
    //     break;
    // case INCREASE_MULTIPLE:
    // case DECREASE_MULTIPLE:
    //     updateCalypsoCardIncreaseOrDecreaseMultiple(
    //         calypsoCard, (CmdCardIncreaseOrDecreaseMultiple) command, apduResponse);
    //     break;
    // case OPEN_SESSION:
    //     updateCalypsoCardOpenSession(calypsoCard, (CmdCardOpenSession) command, apduResponse);
    //     break;
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
    default:
        throw IllegalStateException("Unknown command reference.");
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

}
}
}
