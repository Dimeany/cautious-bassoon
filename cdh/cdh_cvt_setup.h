#ifndef CDH_CURRENT_VALUE_TABLE_SETUP_H
#define CDH_CURRENT_VALUE_TABLE_SETUP_H

#include "cdh_cvt.h"

// Forward declare CurrentValueTable instances
extern cdh::CurrentValueTable<adcs::ADCSHealth> cvt_adcs;
extern cdh::CurrentValueTable<gps::GpsHealth> cvt_gps;
extern cdh::CurrentValueTable<DetectorMessages::HealthPacket> cvt_det;
extern cdh::CurrentValueTable<cdh::CdhExactHealth> cvt_cdh_exact;
extern cdh::CurrentValueTable<cdh::CdhImpressHealth> cvt_cdh_impress;
extern cdh::CurrentValueTable<eps::EpsPDUP4Health> cvt_eps_exact;
extern cdh::CurrentValueTable<eps::EpsImpressHealth> cvt_eps_impress;

#endif // CDH_CURRENT_VALUE_TABLE_SETUP_H
