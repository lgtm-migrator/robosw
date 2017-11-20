/* 
 * vehicle_sim.h
 * 
 * Created on: Nov 20, 2017 10:14
 * Description: a simple continuous-time car model
 * 
 * Copyright (c) 2017 Ruixiang Du (rdu)
 */

#ifndef VEHICLE_SIM_H
#define VEHICLE_SIM_H

#include "common/librav_types.h"

namespace librav
{

class VehicleSim
{
  public:
    VehicleSim();
    VehicleSim(Position2Dd init_pos);
    ~VehicleSim() = default;

  private:
    Position2Dd init_pos_;
};
}

#endif /* VEHICLE_SIM_H */
