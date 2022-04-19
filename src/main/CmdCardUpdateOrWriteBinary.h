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

#include <cstdint>
#include <map>
#include <memory>
#include <vector>

/* Calypsonet Terminal Calypso */
#include "CalypsoCard.h"

/* Keyple Card Calypso */
#include "AbstractApduCommand.h"
#include "AbstractCardCommand.h"
#include "CalypsoCardClass.h"

/* Keyple Core Util */
#include "LoggerFactory.h"

namespace keyple {
namespace card {
namespace calypso {

using namespace calypsonet::terminal::calypso::card;
using namespace keyple::core::util::cpp;

using StatusProperties = AbstractApduCommand::StatusProperties;

/**
 * (package-private)<br>
 * Builds the "Update/Write Binary" APDU command.
 *
 * @since 2.1.0
 */
class CmdCardUpdateOrWriteBinary final : public AbstractCardCommand {
public:
    /**
     * (package-private)<br>
     * Constructor.
     *
     * @param isUpdateCommand True if it is an "Update Binary" command, false if it is a "Write
     *        Binary" command.
     * @param calypsoCardClass indicates which CLA byte should be used for the Apdu.
     * @param sfi the sfi to select.
     * @param offset the offset.
     * @param data the data to write.
     * @since 2.1.0
     */
    CmdCardUpdateOrWriteBinary(const bool isUpdateCommand,
                               const CalypsoCardClass calypsoCardClass,
                               const uint8_t sfi,
                               const int offset,
                               const std::vector<uint8_t>& data);

    /**
     * {@inheritDoc}
     *
     * <p>This command modified the contents of the card and therefore uses the session buffer.
     *
     * @return True
     * @since 2.1.0
     */
    bool isSessionBufferUsed() const override;

    /**
     * (package-private)<br>
     *
     * @return The SFI.
     * @since 2.1.0
     */
    uint8_t getSfi() const;

    /**
     * (package-private)<br>
     *
     * @return The offset.
     * @since 2.1.0
     */
    int getOffset() const;

    /**
     * (package-private)<br>
     *
     * @return The data.
     * @since 2.1.0
     */
    const std::vector<uint8_t>& getData() const;

    /**
     * {@inheritDoc}
     *
     * @since 2.1.0
     */
    const std::map<const int, const std::shared_ptr<StatusProperties>>& getStatusTable() const
        override;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger =
        LoggerFactory::getLogger(typeid(CmdCardUpdateOrWriteBinary));

    /**
     *
     */
    static const std::map<const int, const std::shared_ptr<StatusProperties>> STATUS_TABLE;

    /**
     *
     */
    const uint8_t mSfi;

    /**
     *
     */
    const int mOffset;

    /**
     *
     */
    const std::vector<uint8_t> mData;

};

}
}
}
