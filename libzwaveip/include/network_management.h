/*
 * Copyright 2020 Silicon Laboratories Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NODE_ADD_FSM_H_
#define NODE_ADD_FSM_H_
#include "zconnection.h"

/**
 * @defgroup netmgmt Network management
 * @ingroup zwaveip
 *
 * @{
 */
/**
 * Event types the net_mgmt_command_handler() function must be prepared to
 * receive.
 */
enum net_mgmt_evt_codes {
  APPROVE_REQUESTED_KEYS,
  APPROVE_DSK
};

/** The net_mgmt_command_handler() takes these events as arguments */
union evt_handler_struct {
  /** Event dispatched when joining node requests security keys */
  struct {
    enum net_mgmt_evt_codes type;
    uint8_t requested_keys;
    /** Client side authentication requested flag */
    uint8_t csa_requested;
  } requested_keys;
  /** Event dispatched when joining node has sent its public key */
  struct {
    enum net_mgmt_evt_codes type;
    uint8_t input_dsk_length;
    uint8_t dsk[16];
  } dsk_report;
};

/**
 * Initialize the network management module.

 * Call this function once during application startup.
 *
 * @param zconnection Connection to the ZIP Gateway performing network management
 */
void net_mgmt_init(struct zconnection *_zc);

/**
 * Parse the Network Management Inclusion Command Class packets,
 * which are received by the application.
 *
 * @param packet The received packet, starting at the Command Class byte.
 * @param len    Length of the incoming packet
 */
void parse_network_mgmt_inclusion_packet(const uint8_t *packet, uint16_t len);

/**
 *  Called in response to an APPROVE_REQUESTED_KEYS event.
 *
 *  The application chooses the subset of requested keys that are granted
 *  during inclusion, and also signals if the client side authentication is allowed.
 *
 *  @param granted_keys  Bitmask of granted keys. Same format as that in
 *COMMAND_NODE_ADD_KEYS_REPORT/Requested Keys.
 *  @param csa_accepted  Boolean flag, True if CSA is accepted by the
 *application. May only be set to true if CSA was requested.
 *  @param is_key_accepted a boolean flag, True if the requested keys are accepted
 * by the application. False if the requested keys are rejected by the application.
 *
 */
void net_mgmt_grant_keys(uint8_t granted_keys, uint8_t csa_accepted, bool is_key_accepted);

/**
 *   Must be called in response to an APPROVE_DSK event.
 *
 *   The application MAY fill in the zeroed-out Input DSK part of the Full DSK.
 *
 *   @param  input_dsk   The Input DSK received from a user.
 *   @param  len         Length of the input DSK.
 */
void net_mgmt_set_input_dsk(uint8_t *input_dsk, uint8_t len);

/**
 * Called whenever this module needs to notify the application of
 * an event.
 *
 *  This function must be implemented by upper layers
 *
 *  @param  evt     The event to be processed by the application.
 */
void net_mgmt_command_handler(union evt_handler_struct evt);

/**
 * Called by the application to initiate a node inclusion.
 */
void net_mgmt_learn_mode_start(void);

/**
 * Called by the application to abort a node inclusion.
 */
void net_mgmt_abort_inclusion(void);

/**
 * @}
 */

#endif /* NODE_ADD_FSM_H_ */
