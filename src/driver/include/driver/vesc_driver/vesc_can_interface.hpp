/*
 * vesc_can_interface.hpp
 *
 * Created on 6/30/22 10:21 PM
 * Description:
 *
 * Copyright (c) 2022 Ruixiang Du (rdu)
 */

#ifndef ROBOSW_SRC_DRIVER_INCLUDE_VESC_DRIVER_VESC_CAN_INTERFACE_HPP
#define ROBOSW_SRC_DRIVER_INCLUDE_VESC_DRIVER_VESC_CAN_INTERFACE_HPP

#include <mutex>
#include <functional>

#include "driver/async_port/async_can.hpp"
#include "driver/vesc_driver/vesc_state.hpp"

namespace robosw {
class VescCanInterface {
 public:
  using StateUpdatedCallback = std::function<void(const StampedVescState &)>;

 public:
  VescCanInterface() = default;

  bool Connect(const std::string &can, uint8_t vesc_id);
  void Disconnect();

  uint8_t GetVescId() const;

  void SetStateUpdatedCallback(StateUpdatedCallback cb);
  StampedVescState GetLastState() const;

  void RequestFwVersion();
  void RequestState();
  void RequestImuData();

  void SetDutyCycle(double duty_cycle);
  void SetCurrent(double current);
  void SetBrake(double brake);
  void SetSpeed(double speed);
  void SetPosition(double position);
  void SetServo(double servo);

 private:
  std::shared_ptr<AsyncCAN> can_;
  uint8_t vesc_id_;

  mutable std::mutex state_mtx_;
  StampedVescState stamped_state_;
  StateUpdatedCallback state_updated_callback_;

  void HandleCanFrame(const struct can_frame *frame);
};
}  // namespace robosw

#endif  // ROBOSW_SRC_DRIVER_INCLUDE_VESC_DRIVER_VESC_CAN_INTERFACE_HPP
