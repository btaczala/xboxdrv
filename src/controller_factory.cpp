/*
**  Xbox360 USB Gamepad Userspace Driver
**  Copyright (C) 2011 Ingo Ruhnke <grumbel@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "controller_factory.hpp"

#include <stdexcept>

#include "controller/firestorm_dual_controller.hpp"
#include "controller/generic_usb_controller.hpp"
#include "controller/hama_crux_controller.hpp"
#include "controller/logitech_f310_controller.hpp"
#include "controller/playstation3_usb_controller.hpp"
#include "controller/saitek_p2500_controller.hpp"
#include "controller/wiimote_controller.hpp"
#include "controller/xbox360_controller.hpp"
#include "controller/xbox360_wireless_controller.hpp"
#include "controller/xbox_controller.hpp"
#include "controller/xboxone_wireless_controller.hpp"

ControllerPtr
ControllerFactory::create(const XPadDevice& dev_type, libusb_device* dev, const Options& opts)
{
  switch (dev_type.type)
  {
    case GAMEPAD_XBOX360_PLAY_N_CHARGE:
      // FIXME: only trigger this error message in single-instance mode, not in daemon mode
      throw std::runtime_error("The Xbox360 Play&Charge cable is for recharging only, it does not transmit data, "
                               "thus xboxdrv can't support it. You have to get a wireless receiver:\n"
                               "\n"
                               "  * http://www.xbox.com/en-ca/hardware/x/xbox360wirelessgamingreceiver/");
      break;

    case GAMEPAD_XBOX:
    case GAMEPAD_XBOX_MAT:
      return ControllerPtr(new XboxController(dev, opts.detach_kernel_driver));

    case GAMEPAD_XBOX360:
    case GAMEPAD_XBOX360_GUITAR:
      return ControllerPtr(new Xbox360Controller(dev,
                                                 opts.chatpad, opts.chatpad_no_init, opts.chatpad_debug,
                                                 opts.headset,
                                                 opts.headset_debug,
                                                 opts.headset_dump,
                                                 opts.headset_play,
                                                 opts.detach_kernel_driver));
      break;

    case GAMEPAD_XBOX360_WIRELESS:
      return ControllerPtr(new Xbox360WirelessController(dev, opts.wireless_id, opts.detach_kernel_driver));

    case GAMEPAD_XBOXONE_WIRELESS:
      return ControllerPtr(new XboxOneWirelessController(dev, opts.detach_kernel_driver));

    case GAMEPAD_FIRESTORM:
      return ControllerPtr(new FirestormDualController(dev, false, opts.detach_kernel_driver));

    case GAMEPAD_FIRESTORM_VSB:
      return ControllerPtr(new FirestormDualController(dev, true, opts.detach_kernel_driver));

    case GAMEPAD_SAITEK_P2500:
      return ControllerPtr(new SaitekP2500Controller(dev, opts.detach_kernel_driver));

    case GAMEPAD_LOGITECH_F310:
      return ControllerPtr(new LogitechF310Controller(dev, opts.detach_kernel_driver));

    case GAMEPAD_PLAYSTATION3_USB:
      return ControllerPtr(new Playstation3USBController(dev, opts.detach_kernel_driver));

#ifdef HAVE_CWIID
    case GAMEPAD_WIIMOTE:
      return ControllerPtr(new WiimoteController);
#else
    case GAMEPAD_WIIMOTE:
      throw std::runtime_error("libcwiid not found at compile time, Wiimote support is not available");
#endif

    case GAMEPAD_HAMA_CRUX:
      return ControllerPtr(new HamaCruxController(dev, opts.detach_kernel_driver));

    case GAMEPAD_GENERIC_USB:
      {
        Options::GenericUSBSpec spec = opts.find_generic_usb_spec(dev_type.idVendor, dev_type.idProduct);
        return ControllerPtr(new GenericUSBController(dev, spec.m_interface, spec.m_endpoint,
                                                      opts.detach_kernel_driver));
      }

    default:
      assert(false && "unknown gamepad type");
      return {};
  }
}

std::vector<ControllerPtr>
ControllerFactory::create_multiple(const XPadDevice& dev_type, libusb_device* dev, const Options& opts)
{
  std::vector<ControllerPtr> lst;

  switch (dev_type.type)
  {
    case GAMEPAD_XBOX360_PLAY_N_CHARGE:
      // FIXME: only trigger this error message in single-instance mode, not in daemon mode
      throw std::runtime_error("The Xbox360 Play&Charge cable is for recharging only, it does not transmit data, "
                               "thus xboxdrv can't support it. You have to get a wireless receiver:\n"
                               "\n"
                               "  * http://www.xbox.com/en-ca/hardware/x/xbox360wirelessgamingreceiver/");
      break;

    case GAMEPAD_XBOX:
    case GAMEPAD_XBOX_MAT:
      lst.push_back(ControllerPtr(new XboxController(dev, opts.detach_kernel_driver)));
      break;

    case GAMEPAD_XBOX360:
    case GAMEPAD_XBOX360_GUITAR:
      lst.push_back(ControllerPtr(new Xbox360Controller(dev,
                                                        opts.chatpad, opts.chatpad_no_init, opts.chatpad_debug,
                                                        opts.headset,
                                                        opts.headset_debug,
                                                        opts.headset_dump,
                                                        opts.headset_play,
                                                        opts.detach_kernel_driver)));
      break;

    case GAMEPAD_XBOX360_WIRELESS:
      for(int wireless_id = 0; wireless_id < 4; ++wireless_id)
      {
        lst.push_back(ControllerPtr(new Xbox360WirelessController(dev, wireless_id, opts.detach_kernel_driver)));
      }
      break;

    case GAMEPAD_XBOXONE_WIRELESS:
      lst.push_back(ControllerPtr(new XboxOneWirelessController(dev, opts.detach_kernel_driver)));
      break;

    case GAMEPAD_FIRESTORM:
      lst.push_back(ControllerPtr(new FirestormDualController(dev, false, opts.detach_kernel_driver)));
      break;

    case GAMEPAD_FIRESTORM_VSB:
      lst.push_back(ControllerPtr(new FirestormDualController(dev, true, opts.detach_kernel_driver)));
      break;

    case GAMEPAD_SAITEK_P2500:
      lst.push_back(ControllerPtr(new SaitekP2500Controller(dev, opts.detach_kernel_driver)));
      break;

    case GAMEPAD_LOGITECH_F310:
      lst.push_back(ControllerPtr(new LogitechF310Controller(dev, opts.detach_kernel_driver)));
      break;

    case GAMEPAD_PLAYSTATION3_USB:
      lst.push_back(ControllerPtr(new Playstation3USBController(dev, opts.detach_kernel_driver)));
      break;

#ifdef HAVE_CWIID
    case GAMEPAD_WIIMOTE:
      lst.push_back(ControllerPtr(new WiimoteController));
      break;
#else
    case GAMEPAD_WIIMOTE:
      throw std::runtime_error("libcwiid not found at compile time, Wiimote support is not available");
#endif

    case GAMEPAD_HAMA_CRUX:
      lst.push_back(ControllerPtr(new HamaCruxController(dev, opts.detach_kernel_driver)));
      break;

    case GAMEPAD_GENERIC_USB:
      {
        Options::GenericUSBSpec spec = opts.find_generic_usb_spec(dev_type.idVendor, dev_type.idProduct);
        lst.push_back(ControllerPtr(new GenericUSBController(dev, spec.m_interface, spec.m_endpoint,
                                                             opts.detach_kernel_driver)));
      }
      break;

    default:
      assert(false && "unknown gamepad type");
  }

  return lst;
}

/* EOF */
