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

#include "AbstractCardCommand.h"

/* Keyple Card Calypso */
#include "CardAccessForbiddenException.h"
#include "CardCommandException.h"
#include "CardDataAccessException.h"
#include "CardDataOutOfBoundsException.h"
#include "CardIllegalArgumentException.h"
#include "CardIllegalParameterException.h"
#include "CardPinException.h"
#include "CardSecurityContextException.h"
#include "CardSecurityDataException.h"
#include "CardSessionBufferOverflowException.h"
#include "CardTerminatedException.h"
#include "CardUnknownStatusException.h"

namespace keyple {
namespace card {
namespace calypso {

AbstractCardCommand::AbstractCardCommand(const CalypsoCardCommand& commandRef)
: AbstractApduCommand(commandRef) {}

const CalypsoCardCommand& AbstractCardCommand::getCommandRef() const
{
    return AbstractApduCommand::getCommandRef();
}

const std::shared_ptr<CalypsoApduCommandException> buildCommandException(
    const std::type_info& exceptionClass,
    const std::string& message,
    const CardCommand& commandRef,
    const std::shared_ptr<int> statusWord) {

    if (exceptionClass == typeid(CardAccessForbiddenException)) {
        return std::make_shared<CardAccessForbiddenException>(message, commandRef, statusWord);
    } else if (exceptionClass == typeid(CardDataAccessException)) {
        return std::make_shared<CardDataAccessException>(message, commandRef, statusWord);
    } else if (exceptionClass == typeid(CardDataOutOfBoundsException)) {
        return std::make_shared<CardDataOutOfBoundsException>(message, commandRef, statusWord);
    } else if (exceptionClass == typeid(CardIllegalArgumentException)) {
        return std::make_shared<CardIllegalArgumentException>(message, commandRef);
    } else if (exceptionClass == typeid(CardIllegalParameterException)) {
        return std::make_shared<CardIllegalParameterException>(message, commandRef, statusWord);
    } else if (exceptionClass == typeid(CardPinException)) {
        return std::make_shared<CardPinException>(message, commandRef, statusWord);
    } else if (exceptionClass == typeid(CardSecurityContextException)) {
        return std::make_shared<CardSecurityContextException>(message, commandRef, statusWord);
    } else if (exceptionClass == typeid(CardSecurityDataException)) {
        return std::make_shared<CardSecurityDataException>(message, commandRef, statusWord);
    } else if (exceptionClass == typeid(CardSessionBufferOverflowException)) {
        return std::make_shared<CardSessionBufferOverflowException>(message, commandRef, statusWord);
    } else if (exceptionClass == typeid(CardTerminatedException)) {
        return std::make_shared<CardTerminatedException>(message, commandRef, statusWord);
    } else {
        return std::make_shared<CardUnknownStatusException>(message, commandRef, statusWord);
    }
}

AbstractCardCommand& AbstractCardCommand::setApduResponse(
    const std::shared_ptr<ApduResponseApi> apduResponse)
{
    return AbstractApduCommand::setApduResponse(apduResponse);
}

void AbstractCardCommand::checkStatus()
{
    try {
        AbstractApduCommand::checkStatus();
    } catch (const CalypsoApduCommandException& e) {
        throw CardCommandException(e);
    }
}

}
}
}
