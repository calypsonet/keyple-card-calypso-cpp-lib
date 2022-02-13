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

#include "CalypsoCardClass.h"

namespace keyple {
namespace card {
namespace calypso {

const CalypsoCardClass CalypsoCardClass::LEGACY(0x94);
const CalypsoCardClass CalypsoCardClass::LEGACY_STORED_VALUE(0xFA);
const CalypsoCardClass CalypsoCardClass::ISO(0x00);

CalypsoCardClass::CalypsoCardClass(const uint8_t cla) : mCla(cla) {}

uint8_t CalypsoCardClass::getValue() const
{
    return mCla;
}

}
}
}
