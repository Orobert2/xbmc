/*
 *  Copyright (C) 2021- Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "network/IWSDiscovery.h"
#include "threads/CriticalSection.h"
#include "threads/SingleLock.h"

#include "platform/posix/filesystem/SMBWSDiscoveryListener.h"

#include <memory>
#include <string>
#include <vector>

class CFileItemList;

namespace WSDiscovery
{
class CWSDiscoveryListenerUDP;
}

namespace WSDiscovery
{
struct wsd_req_info
{
  std::string action; // ToDo: Action probably isnt required to be stored
  std::string msgid;
  std::string types; // ToDo: Types may not be needed.
  std::string address;
  std::string xaddrs;

  bool operator==(const wsd_req_info& item) const
  {
    return ((item.xaddrs == xaddrs) && (item.address == address));
  }
};

class CWSDiscoveryPosix : public WSDiscovery::IWSDiscovery
{
public:
  CWSDiscoveryPosix();

  // IWSDiscovery interface methods
  ~CWSDiscoveryPosix() override;
  bool StartServices() override;
  bool StopServices() override;
  bool IsRunning() override;

  /*
   * Get List of smb servers found by WSD
   * out    (CFileItemList&) List of fileitems populated with smb addresses
   * return (bool) true if >0 WSD addresses found
  */
  bool GetServerList(CFileItemList& items);

  const long long GetInstanceID() { return wsd_instance_id; }

  /*
   * Set List of WSD info request
   * out    (vector<wsd_req_info>) vector of WSD responses received
   * return void
  */
  void SetItems(std::vector<WSDiscovery::wsd_req_info> entries);

private:
  CCriticalSection m_critWSD;

  /*
   * As per docs - Pg32 - http://specs.xmlsoap.org/ws/2005/04/discovery/ws-discovery.pdf
   * MUST be incremented by >= 1 each time the service has gone down, lost state,
   * and came back up again. SHOULD NOT be incremented otherwise. Means to set
   * this value include, but are not limited to:
   * • A counter that is incremented on each 'cold' boot
   * • The boot time of the service, expressed as seconds elapsed since midnight
   * January 1, 1970 
   *
   * Our implementation will only set this on creation of the class
   * We arent providing services to clients, so this should be ok.
   */
  long long wsd_instance_id;

  // WSD UDP daemon
  std::unique_ptr<WSDiscovery::CWSDiscoveryListenerUDP> m_WSDListenerUDP;

  std::vector<WSDiscovery::wsd_req_info> m_vecWSDInfo;
};
} // namespace WSDiscovery
