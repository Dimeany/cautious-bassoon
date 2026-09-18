#pragma once

#include <cstddef>
#include <cstdint>

#pragma pack(push, 1)

namespace CubeSpace {
template<typename T> struct MessageSize;
template<typename T> struct GetID;
template<typename T> struct SetID;

template<typename T> constexpr size_t MessageSize_v = MessageSize<T>::value;
template<typename T> constexpr int GetID_v = GetID<T>::value;
template<typename T> constexpr int SetID_v = SetID<T>::value;

enum class FileTypeEnumerationValues : uint8_t {
  TelemetryLog = 2,
  JPGImage,
  BMPImage,
  Index = 15,
};

enum class FileUploadDestinationEnumerationValues : uint8_t {
  EEPROM = 2,
  Flashprogram1,
  Flashprogram2,
  Flashprogram3,
  Flashprogram4,
  Flashprogram5,
  Flashprogram6,
  Flashprogram7,
  SDUserfile1,
  SDUserfile2,
  SDUserfile3,
  SDUserfile4,
  SDUserfile5,
  SDUserfile6,
  SDUserfile7,
  SDUserfile8,
};

enum class ResetCauseEnumerationValues : uint8_t {
  PowerOnReset,
  BrownOutDetectedonRegulatedPower,
  BrownOutDetectedonUnregulatedPower,
  ExternalWatchdogReset,
  ExternalReset,
  WatchdogReset,
  LockupSystemReset,
  LockupReset,
  SystemRequestReset,
  BackupBrownOut,
  BackupModeReset,
  BackupModeRSTandBackupBrownOutVddRegulated,
  BackupModeRSTandBackupBrownOutVddRegulatedandBrownOutRegulated,
  BackupModeRSTandWatchdogReset,
  BackupBrownOutBuvinandSystemRequestReset,
  UnkownResetCause,
};

enum class BootCauseEnumerationValues : uint8_t {
  Unexpected,
  NotUsed0,
  CommunicationsTimeout,
  Commanded,
  NotUsed1,
  SRAMLatchup,
};

enum class BootProgramsListEnumerationValues : uint8_t {
  InternalFlashProgram = 1,
  Bootloader,
};

enum class BootStatusEnumerationValues : uint8_t {
  NewSelection,
  BootSuccess,
  Number1Failedbootattempt,
  Number2Failedbootattempts,
  Number3Failedbootattempts,
};

enum class TcErrorReasonEnumerationValues : uint8_t {
  NoError,
  InvalidTC,
  IncorrectLength,
  IncorrectParameter,
  CRCcheckfailed,
};

enum class BootSetProgramsListEnumerationValues : uint8_t {
  InternalFlashProgram = 1,
};

enum class ProgramsListEnumerationValues : uint8_t {
  Bootloader,
  InternalFlashProgram,
  EEPROM,
  ExternalFlashProgram1,
  ExternalFlashProgram2,
  ExternalFlashProgram3,
  ExternalFlashProgram4,
  ExternalFlashProgram5,
  ExternalFlashProgram6,
  ExternalFlashProgram7,
  SDUserfile1,
  SDUserfile2,
  SDUserfile3,
  SDUserfile4,
  SDUserfile5,
  SDUserfile6,
  SDUserfile7,
  SDUserfile8,
};

enum class AdcsRunModeEnumerationValues : uint8_t {
  Off,
  Enabled,
  Triggered,
  Simulation,
};

enum class ConModeSelectEnumerationValues : uint8_t {
  Nocontrol,
  Detumblingcontrol,
  YThomsonspin,
  YWheelmomentumstabilizedInitialPitchAcquisition,
  YWheelmomentumstabilizedSteadyState,
  XYZWheelcontrol,
  Rwheelsuntrackingcontrol,
  Rwheeltargettrackingcontrol,
  VeryFastspinDetumblingcontrol,
  FastspinDetumblingcontrol,
  UserSpecificControlMode1,
  UserSpecificControlMode2,
  StopRwheels,
  UserCodedControlMode,
  Suntrackingyaworrollonlywheelcontrolmode,
  TargettrackingyawonlywheelcontrolMode,
};

enum class EstimModeSelectEnumerationValues : uint8_t {
  Noattitudeestimation,
  MEMSratesensing,
  Magnetometerratefilter,
  Magnetometerratefilterwithpitchestimation,
  MagnetometerandFinesunTRIADalgorithm,
  FullstateEKF,
  MEMSgyroEKF,
  UserCodedEstimationMode,
};

enum class GpsSolutionStatusEnumerationValues : uint8_t {
  Solutioncomputed,
  Insufficientobservations,
  Noconvergence,
  Singularityatparametersmatrix,
  Covariancetraceexceedsmaximum,
  Notyetconvergedfromcoldstart,
  Heightorvelocitylimitsexceeded,
  Varianceexceedslimits,
  Largeresidualsmakepositionunreliable,
  Calculatingcomparisontouserprovided,
  Thefixedpositionisinvalid,
  Positiontypeisunauthorized,
};

enum class Asgp4ModeSelectEnumerationValues : uint8_t {
  Off,
  Trigger,
  Background,
  Augment,
};

enum class MagModeValEnumerationValues : uint8_t {
  MainMTMSampledThroughSignal,
  RedundantMTMSampledThroughSignal,
  MainMTMSampledThroughMotor,
  None,
};

enum class CamSelectEnumerationValues : uint8_t {
  Cam1,
  Cam2,
  Star,
};

enum class ImSizeEnumerationValues : uint8_t {
  Size0,
  Size1,
  Size2,
  Size3,
  Size4,
};

enum class JpgConvertResultEnumerationValues : uint8_t {
  NothingConvertedYet,
  Success,
  FileLoadError,
  Busy,
};

enum class CaptureResultEnumerationValues : uint8_t {
  Startup,
  Pending,
  Success,
  SuccessShift,
  Timeout,
  SRAMError,
};

enum class DetectResultEnumerationValues : uint8_t {
  Startup,
  NoDetect,
  Pending,
  TooManyEdges,
  TooFewEdges,
  BadFit,
  SunNotFound,
  Success,
};

enum class StarIDModeValEnumerationValues : uint8_t {
  TrackingMode,
  LostMode,
};

enum class ExecutionWaypointsEnumerationValues : uint8_t {
  Init,
  Idle,
  SensorActuatorCommunications,
  ADCSUpdate,
  PeripheralPowercommandsoverI2C,
  CPUTemperatureSampling,
  ImageDownload,
  ImageCompression,
  SavingImagetoSDCard,
  Logging,
  LogFileCompression,
  SavingLogtoSDCard,
  Writingtoflash,
};

enum class AsgpErrorEnumerationValues : uint8_t {
  Off,
  UnixTime,
  Position,
  Overflow,
};

enum class ImSaveStatusEnumerationValues : uint8_t {
  NoError,
  Timeoutwaitingforsensortobecomeavailable,
  Timeoutwaitingfornextframetobecomeready,
  Checksummismatchbetweendownloadedframeandunitframe,
  ErrorwritingtoSDcard,
};

enum class AxisSelectEnumerationValues : uint8_t {
  PositiveX,
  NegativeX,
  PositiveY,
  NegativeY,
  PositiveZ,
  NegativeZ,
  NotUsed0,
};

enum class PowerSelectEnumerationValues : uint8_t {
  Off,
  On,
  Powerstatekeptthesame,
};

enum class AsgpFilterEnumerationValues : uint8_t {
  LPF,
  Average,
};

enum class AcpProgramTypeEnumerationValues : uint8_t {
  Number3AxisACP,
  YMomentumACP,
};

enum class SpecialConSelectEnumerationValues : uint8_t {
  None,
  ZaxisSunpointing,
  GEOTracking,
  Inertialpointing,
  YaxisSunpointing,
};

enum class CsCamTypeEnumerationValues : uint8_t {
  SunSensor,
  NadirSensor,
};

enum class GpsSelectEnumerationValues : uint8_t {
  NoGPSSupport,
  NovatelGPS,
  SkyfoxGPS,
};

enum class GpioPortEnumerationValues : uint8_t {
  PortA,
  PortB,
  PortC,
  PortD,
  PortE,
  PortF,
};

enum class GpioPortPinEnumerationValues : uint8_t {
  Pin0,
  Pin1,
  Pin2,
  Pin3,
  Pin4,
  Pin5,
  Pin6,
  Pin7,
  Pin8,
  Pin9,
  Pin10,
  Pin11,
  Pin12,
  Pin13,
  Pin14,
  Pin15,
};

enum class SdLogSelectEnumerationValues : uint8_t {
  PrimarySDCard,
  SecondarySDCard,
};

struct ResetCommandFormat {
  uint8_t Magicnumber;
};
static_assert(sizeof(ResetCommandFormat) == 1);
template <> struct MessageSize<ResetCommandFormat> { static constexpr size_t value = 1; };
template <> struct SetID<ResetCommandFormat> { static constexpr int value = 1; };

struct ResetLogPointerCommandFormat {
};
static_assert(sizeof(ResetLogPointerCommandFormat) == 1);
template <> struct MessageSize<ResetLogPointerCommandFormat> { static constexpr size_t value = 0; };
template <> struct SetID<ResetLogPointerCommandFormat> { static constexpr int value = 4; };

struct AdvanceLogPointerCommandFormat {
};
static_assert(sizeof(AdvanceLogPointerCommandFormat) == 1);
template <> struct MessageSize<AdvanceLogPointerCommandFormat> { static constexpr size_t value = 0; };
template <> struct SetID<AdvanceLogPointerCommandFormat> { static constexpr int value = 5; };

struct ResetBootRegistersCommandFormat {
};
static_assert(sizeof(ResetBootRegistersCommandFormat) == 1);
template <> struct MessageSize<ResetBootRegistersCommandFormat> { static constexpr size_t value = 0; };
template <> struct SetID<ResetBootRegistersCommandFormat> { static constexpr int value = 6; };

struct FormatSDcardCommandFormat {
  uint8_t MagicNumber;
};
static_assert(sizeof(FormatSDcardCommandFormat) == 1);
template <> struct MessageSize<FormatSDcardCommandFormat> { static constexpr size_t value = 1; };
template <> struct SetID<FormatSDcardCommandFormat> { static constexpr int value = 33; };

struct EraseFileCommandFormat {
  FileTypeEnumerationValues FileType : 8;
  uint8_t FileCounter;
  bool EraseAll : 1;
};
static_assert(sizeof(EraseFileCommandFormat) == 3);
template <> struct MessageSize<EraseFileCommandFormat> { static constexpr size_t value = 3; };
template <> struct SetID<EraseFileCommandFormat> { static constexpr int value = 108; };

struct LoadFileDownloadBlockCommandFormat {
  FileTypeEnumerationValues FileType : 8;
  uint8_t Counter;
  uint32_t Offset;
  uint16_t BlockLength;
};
static_assert(sizeof(LoadFileDownloadBlockCommandFormat) == 8);
template <> struct MessageSize<LoadFileDownloadBlockCommandFormat> { static constexpr size_t value = 8; };
template <> struct SetID<LoadFileDownloadBlockCommandFormat> { static constexpr int value = 112; };

struct AdvanceFileListReadPointerCommandFormat {
};
static_assert(sizeof(AdvanceFileListReadPointerCommandFormat) == 1);
template <> struct MessageSize<AdvanceFileListReadPointerCommandFormat> { static constexpr size_t value = 0; };
template <> struct SetID<AdvanceFileListReadPointerCommandFormat> { static constexpr int value = 113; };

struct InitiateFileUploadCommandFormat {
  FileUploadDestinationEnumerationValues Destination : 8;
  uint8_t BlockSize;
};
static_assert(sizeof(InitiateFileUploadCommandFormat) == 2);
template <> struct MessageSize<InitiateFileUploadCommandFormat> { static constexpr size_t value = 2; };
template <> struct SetID<InitiateFileUploadCommandFormat> { static constexpr int value = 114; };

struct FileUploadPacketCommandFormat {
  uint16_t PacketNumber;
  char FileBytes[20];
};
static_assert(sizeof(FileUploadPacketCommandFormat) == 22);
template <> struct MessageSize<FileUploadPacketCommandFormat> { static constexpr size_t value = 22; };
template <> struct SetID<FileUploadPacketCommandFormat> { static constexpr int value = 115; };

struct FinalizeUploadBlockCommandFormat {
  FileUploadDestinationEnumerationValues Destination : 8;
  uint32_t Offset;
  uint16_t BlockLength;
};
static_assert(sizeof(FinalizeUploadBlockCommandFormat) == 7);
template <> struct MessageSize<FinalizeUploadBlockCommandFormat> { static constexpr size_t value = 7; };
template <> struct SetID<FinalizeUploadBlockCommandFormat> { static constexpr int value = 116; };

struct ResetUploadBlockCommandFormat {
};
static_assert(sizeof(ResetUploadBlockCommandFormat) == 1);
template <> struct MessageSize<ResetUploadBlockCommandFormat> { static constexpr size_t value = 0; };
template <> struct SetID<ResetUploadBlockCommandFormat> { static constexpr int value = 117; };

struct ResetFileListReadPointerCommandFormat {
};
static_assert(sizeof(ResetFileListReadPointerCommandFormat) == 1);
template <> struct MessageSize<ResetFileListReadPointerCommandFormat> { static constexpr size_t value = 0; };
template <> struct SetID<ResetFileListReadPointerCommandFormat> { static constexpr int value = 118; };

struct InitiateDownloadBurstCommandFormat {
  uint8_t MessageLength;
  bool IgnoreHoleMap : 1;
};
static_assert(sizeof(InitiateDownloadBurstCommandFormat) == 2);
template <> struct MessageSize<InitiateDownloadBurstCommandFormat> { static constexpr size_t value = 2; };
template <> struct SetID<InitiateDownloadBurstCommandFormat> { static constexpr int value = 119; };

struct IdentificationTelemetryFormat {
  uint8_t Nodetype;
  uint8_t Interfaceversion;
  uint8_t FirmwareversionMajor;
  uint8_t FirmwareversionMinor;
  uint16_t Runtimeseconds;
  uint16_t Runtimemilliseconds;
};
static_assert(sizeof(IdentificationTelemetryFormat) == 8);
template <> struct MessageSize<IdentificationTelemetryFormat> { static constexpr size_t value = 8; };
template <> struct GetID<IdentificationTelemetryFormat> { static constexpr int value = 128; };

struct BootAndRunningProgramStatusTelemetryFormat {
  ResetCauseEnumerationValues CauseofMCUReset : 4;
  BootCauseEnumerationValues BootCause : 4;
  uint16_t BootCounter;
  BootProgramsListEnumerationValues BootProgramIndex : 8;
  uint8_t FirmwareversionMajor;
  uint8_t FirmwareversionMinor;
};
static_assert(sizeof(BootAndRunningProgramStatusTelemetryFormat) == 6);
template <> struct MessageSize<BootAndRunningProgramStatusTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<BootAndRunningProgramStatusTelemetryFormat> { static constexpr int value = 129; };

struct BootIndexandStatusTelemetryFormat {
  BootProgramsListEnumerationValues ProgramIndex : 8;
  BootStatusEnumerationValues BootStatus : 8;
};
static_assert(sizeof(BootIndexandStatusTelemetryFormat) == 2);
template <> struct MessageSize<BootIndexandStatusTelemetryFormat> { static constexpr size_t value = 2; };
template <> struct GetID<BootIndexandStatusTelemetryFormat> { static constexpr int value = 130; };

struct LastLoggedEventTelemetryFormat {
  uint32_t TimeofEvent;
  uint8_t EventID;
  uint8_t EventParameter;
};
static_assert(sizeof(LastLoggedEventTelemetryFormat) == 6);
template <> struct MessageSize<LastLoggedEventTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<LastLoggedEventTelemetryFormat> { static constexpr int value = 141; };

struct SRAMLatchupcountersTelemetryFormat {
  uint16_t SRAM1latchups;
  uint16_t SRAM2latchups;
  char unused_0[2];
};
static_assert(sizeof(SRAMLatchupcountersTelemetryFormat) == 6);
template <> struct MessageSize<SRAMLatchupcountersTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<SRAMLatchupcountersTelemetryFormat> { static constexpr int value = 142; };

struct EDACErrorCountersTelemetryFormat {
  uint16_t SingleSRAMupsets;
  uint16_t DoubleSRAMupsets;
  uint16_t MultipleSRAMupsets;
};
static_assert(sizeof(EDACErrorCountersTelemetryFormat) == 6);
template <> struct MessageSize<EDACErrorCountersTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<EDACErrorCountersTelemetryFormat> { static constexpr int value = 143; };

struct CommunicationStatusTelemetryFormat {
  uint16_t Telecommandcounter;
  uint16_t Telemetryrequestcounter;
  bool Telecommandbufferoverrun : 1;
  bool UARTprotocolerror : 1;
  bool UARTincompletemessage : 1;
  bool I2Ctelemetryerror : 1;
  bool I2Ctelecommandbuffererror : 1;
  bool CANtelecommandbuffererror : 1;
  char unused_0[1];
};
static_assert(sizeof(CommunicationStatusTelemetryFormat) == 6);
template <> struct MessageSize<CommunicationStatusTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<CommunicationStatusTelemetryFormat> { static constexpr int value = 144; };

struct SDcardformateraseprogressTelemetryFormat {
  bool FormatBusy : 1;
  bool EraseAllBusy : 1;
};
static_assert(sizeof(SDcardformateraseprogressTelemetryFormat) == 1);
template <> struct MessageSize<SDcardformateraseprogressTelemetryFormat> { static constexpr size_t value = 1; };
template <> struct GetID<SDcardformateraseprogressTelemetryFormat> { static constexpr int value = 234; };

struct TelecommandAcknowledgeTelemetryFormat {
  uint8_t LastTCID;
  bool Processedflag : 1;
  char : 0;
  TcErrorReasonEnumerationValues TCerrorstatus : 8;
  uint8_t TCparametererrorindex;
};
static_assert(sizeof(TelecommandAcknowledgeTelemetryFormat) == 4);
template <> struct MessageSize<TelecommandAcknowledgeTelemetryFormat> { static constexpr size_t value = 4; };
template <> struct GetID<TelecommandAcknowledgeTelemetryFormat> { static constexpr int value = 240; };

struct FileDownloadBufferwithFileContentsTelemetryFormat {
  uint16_t Packetcounter;
  char Filebytes[20];
};
static_assert(sizeof(FileDownloadBufferwithFileContentsTelemetryFormat) == 22);
template <> struct MessageSize<FileDownloadBufferwithFileContentsTelemetryFormat> { static constexpr size_t value = 22; };
template <> struct GetID<FileDownloadBufferwithFileContentsTelemetryFormat> { static constexpr int value = 241; };

struct DownloadBlockReadyTelemetryFormat {
  bool Ready : 1;
  bool ParameterError : 1;
  char : 0;
  uint16_t BlockCRC16Checksum;
  uint16_t BlockLength;
};
static_assert(sizeof(DownloadBlockReadyTelemetryFormat) == 5);
template <> struct MessageSize<DownloadBlockReadyTelemetryFormat> { static constexpr size_t value = 5; };
template <> struct GetID<DownloadBlockReadyTelemetryFormat> { static constexpr int value = 242; };

struct FileInformationTelemetryFormat {
  FileTypeEnumerationValues FileType : 4;
  bool BusyUpdating : 1;
  char : 0;
  uint8_t FileCtr;
  uint32_t FileSize;
  uint32_t FileDateandTime;
  uint16_t FileCRC16Checksum;
};
static_assert(sizeof(FileInformationTelemetryFormat) == 12);
template <> struct MessageSize<FileInformationTelemetryFormat> { static constexpr size_t value = 12; };
template <> struct GetID<FileInformationTelemetryFormat> { static constexpr int value = 243; };

struct InitializeUploadCompleteTelemetryFormat {
  bool Busy : 1;
};
static_assert(sizeof(InitializeUploadCompleteTelemetryFormat) == 1);
template <> struct MessageSize<InitializeUploadCompleteTelemetryFormat> { static constexpr size_t value = 1; };
template <> struct GetID<InitializeUploadCompleteTelemetryFormat> { static constexpr int value = 244; };

struct UploadBlockCompleteTelemetryFormat {
  bool Busy : 1;
  bool Error : 1;
};
static_assert(sizeof(UploadBlockCompleteTelemetryFormat) == 1);
template <> struct MessageSize<UploadBlockCompleteTelemetryFormat> { static constexpr size_t value = 1; };
template <> struct GetID<UploadBlockCompleteTelemetryFormat> { static constexpr int value = 245; };

struct BlockChecksumTelemetryFormat {
  uint16_t Checksum;
};
static_assert(sizeof(BlockChecksumTelemetryFormat) == 2);
template <> struct MessageSize<BlockChecksumTelemetryFormat> { static constexpr size_t value = 2; };
template <> struct GetID<BlockChecksumTelemetryFormat> { static constexpr int value = 246; };

struct CacheenabledstateMessageFormat {
  bool Enabledstate : 1;
};
static_assert(sizeof(CacheenabledstateMessageFormat) == 1);
template <> struct MessageSize<CacheenabledstateMessageFormat> { static constexpr size_t value = 1; };
template <> struct SetID<CacheenabledstateMessageFormat> { static constexpr int value = 3; };
template <> struct GetID<CacheenabledstateMessageFormat> { static constexpr int value = 131; };

struct SRAMScrubParametersMessageFormat {
  uint16_t ScrubSize;
};
static_assert(sizeof(SRAMScrubParametersMessageFormat) == 2);
template <> struct MessageSize<SRAMScrubParametersMessageFormat> { static constexpr size_t value = 2; };
template <> struct SetID<SRAMScrubParametersMessageFormat> { static constexpr int value = 8; };
template <> struct GetID<SRAMScrubParametersMessageFormat> { static constexpr int value = 134; };

struct CurrentUnixTimeMessageFormat {
  uint32_t CurrentUnixTime;
  uint16_t Milliseconds;
};
static_assert(sizeof(CurrentUnixTimeMessageFormat) == 6);
template <> struct MessageSize<CurrentUnixTimeMessageFormat> { static constexpr size_t value = 6; };
template <> struct SetID<CurrentUnixTimeMessageFormat> { static constexpr int value = 2; };
template <> struct GetID<CurrentUnixTimeMessageFormat> { static constexpr int value = 140; };

struct UnixTimeSavetoFlashMessageFormat {
  bool SaveNow : 1;
  bool SaveOnUpdate : 1;
  bool SavePeriodic : 1;
  char : 0;
  uint8_t Period;
};
static_assert(sizeof(UnixTimeSavetoFlashMessageFormat) == 2);
template <> struct MessageSize<UnixTimeSavetoFlashMessageFormat> { static constexpr size_t value = 2; };
template <> struct SetID<UnixTimeSavetoFlashMessageFormat> { static constexpr int value = 9; };
template <> struct GetID<UnixTimeSavetoFlashMessageFormat> { static constexpr int value = 145; };

struct HoleMap1MessageFormat {
  char HoleMap[16];
};
static_assert(sizeof(HoleMap1MessageFormat) == 16);
template <> struct MessageSize<HoleMap1MessageFormat> { static constexpr size_t value = 16; };
template <> struct SetID<HoleMap1MessageFormat> { static constexpr int value = 120; };
template <> struct GetID<HoleMap1MessageFormat> { static constexpr int value = 247; };

struct HoleMap2MessageFormat {
  char HoleMap[16];
};
static_assert(sizeof(HoleMap2MessageFormat) == 16);
template <> struct MessageSize<HoleMap2MessageFormat> { static constexpr size_t value = 16; };
template <> struct SetID<HoleMap2MessageFormat> { static constexpr int value = 121; };
template <> struct GetID<HoleMap2MessageFormat> { static constexpr int value = 248; };

struct HoleMap3MessageFormat {
  char HoleMap[16];
};
static_assert(sizeof(HoleMap3MessageFormat) == 16);
template <> struct MessageSize<HoleMap3MessageFormat> { static constexpr size_t value = 16; };
template <> struct SetID<HoleMap3MessageFormat> { static constexpr int value = 122; };
template <> struct GetID<HoleMap3MessageFormat> { static constexpr int value = 249; };

struct HoleMap4MessageFormat {
  char HoleMap[16];
};
static_assert(sizeof(HoleMap4MessageFormat) == 16);
template <> struct MessageSize<HoleMap4MessageFormat> { static constexpr size_t value = 16; };
template <> struct SetID<HoleMap4MessageFormat> { static constexpr int value = 123; };
template <> struct GetID<HoleMap4MessageFormat> { static constexpr int value = 250; };

struct HoleMap5MessageFormat {
  char HoleMap[16];
};
static_assert(sizeof(HoleMap5MessageFormat) == 16);
template <> struct MessageSize<HoleMap5MessageFormat> { static constexpr size_t value = 16; };
template <> struct SetID<HoleMap5MessageFormat> { static constexpr int value = 124; };
template <> struct GetID<HoleMap5MessageFormat> { static constexpr int value = 251; };

struct HoleMap6MessageFormat {
  char HoleMap[16];
};
static_assert(sizeof(HoleMap6MessageFormat) == 16);
template <> struct MessageSize<HoleMap6MessageFormat> { static constexpr size_t value = 16; };
template <> struct SetID<HoleMap6MessageFormat> { static constexpr int value = 125; };
template <> struct GetID<HoleMap6MessageFormat> { static constexpr int value = 252; };

struct HoleMap7MessageFormat {
  char HoleMap[16];
};
static_assert(sizeof(HoleMap7MessageFormat) == 16);
template <> struct MessageSize<HoleMap7MessageFormat> { static constexpr size_t value = 16; };
template <> struct SetID<HoleMap7MessageFormat> { static constexpr int value = 126; };
template <> struct GetID<HoleMap7MessageFormat> { static constexpr int value = 253; };

struct HoleMap8MessageFormat {
  char HoleMap[16];
};
static_assert(sizeof(HoleMap8MessageFormat) == 16);
template <> struct MessageSize<HoleMap8MessageFormat> { static constexpr size_t value = 16; };
template <> struct SetID<HoleMap8MessageFormat> { static constexpr int value = 127; };
template <> struct GetID<HoleMap8MessageFormat> { static constexpr int value = 254; };

struct ClearErrorFlagsCommandFormat {
};
static_assert(sizeof(ClearErrorFlagsCommandFormat) == 1);
template <> struct MessageSize<ClearErrorFlagsCommandFormat> { static constexpr size_t value = 0; };
template <> struct SetID<ClearErrorFlagsCommandFormat> { static constexpr int value = 7; };

struct SetBootIndexCommandFormat {
  BootSetProgramsListEnumerationValues ProgramIndex : 8;
};
static_assert(sizeof(SetBootIndexCommandFormat) == 1);
template <> struct MessageSize<SetBootIndexCommandFormat> { static constexpr size_t value = 1; };
template <> struct SetID<SetBootIndexCommandFormat> { static constexpr int value = 100; };

struct RunSelectedProgramCommandFormat {
};
static_assert(sizeof(RunSelectedProgramCommandFormat) == 1);
template <> struct MessageSize<RunSelectedProgramCommandFormat> { static constexpr size_t value = 0; };
template <> struct SetID<RunSelectedProgramCommandFormat> { static constexpr int value = 101; };

struct ReadProgramInformationCommandFormat {
  ProgramsListEnumerationValues ProgramIndex : 8;
};
static_assert(sizeof(ReadProgramInformationCommandFormat) == 1);
template <> struct MessageSize<ReadProgramInformationCommandFormat> { static constexpr size_t value = 1; };
template <> struct SetID<ReadProgramInformationCommandFormat> { static constexpr int value = 102; };

struct CopyProgramtoInternalFlashCommandFormat {
  ProgramsListEnumerationValues SourceProgramIndex : 8;
  uint8_t Bootloaderoverwriteflag;
};
static_assert(sizeof(CopyProgramtoInternalFlashCommandFormat) == 2);
template <> struct MessageSize<CopyProgramtoInternalFlashCommandFormat> { static constexpr size_t value = 2; };
template <> struct SetID<CopyProgramtoInternalFlashCommandFormat> { static constexpr int value = 103; };

struct BootloaderStateTelemetryFormat {
  uint16_t Uptimes;
  bool SRAM1isenabled : 1;
  bool SRAM2isenabled : 1;
  bool SRAMLatchupErroroccurredandcouldnotberecovered : 1;
  bool SRAMLatchupOccurredbutrecoveredafterpowercycle : 1;
  bool SDcardinitialisationerror : 1;
  bool SDcardreaderror : 1;
  bool SDcardwriteerror : 1;
  bool ExternalFlashError : 1;
  bool InternalFlashError : 1;
  bool EEPROMError : 1;
  bool BootRegisterCorrupt : 1;
  bool CommunicationsErrorwithRadio : 1;
  char unused_0[2];
};
static_assert(sizeof(BootloaderStateTelemetryFormat) == 6);
template <> struct MessageSize<BootloaderStateTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<BootloaderStateTelemetryFormat> { static constexpr int value = 132; };

struct ProgramInformationTelemetryFormat {
  ProgramsListEnumerationValues ProgramIndex : 8;
  bool Busy : 1;
  char : 0;
  uint32_t FileSize;
  uint16_t Checksum;
};
static_assert(sizeof(ProgramInformationTelemetryFormat) == 8);
template <> struct MessageSize<ProgramInformationTelemetryFormat> { static constexpr size_t value = 8; };
template <> struct GetID<ProgramInformationTelemetryFormat> { static constexpr int value = 232; };

struct CopyToInternalFlashProgressTelemetryFormat {
  bool Busy : 1;
  bool Error : 1;
};
static_assert(sizeof(CopyToInternalFlashProgressTelemetryFormat) == 1);
template <> struct MessageSize<CopyToInternalFlashProgressTelemetryFormat> { static constexpr size_t value = 1; };
template <> struct GetID<CopyToInternalFlashProgressTelemetryFormat> { static constexpr int value = 233; };

struct DeployMagnetometerBoomCommandFormat {
  uint8_t Timeout;
};
static_assert(sizeof(DeployMagnetometerBoomCommandFormat) == 1);
template <> struct MessageSize<DeployMagnetometerBoomCommandFormat> { static constexpr size_t value = 1; };
template <> struct SetID<DeployMagnetometerBoomCommandFormat> { static constexpr int value = 7; };

struct ADCSRunModeCommandFormat {
  AdcsRunModeEnumerationValues Enabled : 8;
};
static_assert(sizeof(ADCSRunModeCommandFormat) == 1);
template <> struct MessageSize<ADCSRunModeCommandFormat> { static constexpr size_t value = 1; };
template <> struct SetID<ADCSRunModeCommandFormat> { static constexpr int value = 10; };

struct ClearErrorsCommandFormat {
  bool ADCSErrorFlags : 1;
  bool HKErrorFlags : 1;
};
static_assert(sizeof(ClearErrorsCommandFormat) == 1);
template <> struct MessageSize<ClearErrorsCommandFormat> { static constexpr size_t value = 1; };
template <> struct SetID<ClearErrorsCommandFormat> { static constexpr int value = 12; };

struct SetAttitudeControlModeCommandFormat {
  ConModeSelectEnumerationValues ControlMode : 8;
  uint16_t Timeout;
};
static_assert(sizeof(SetAttitudeControlModeCommandFormat) == 3);
template <> struct MessageSize<SetAttitudeControlModeCommandFormat> { static constexpr size_t value = 3; };
template <> struct SetID<SetAttitudeControlModeCommandFormat> { static constexpr int value = 13; };

struct SetAttitudeEstimationModeCommandFormat {
  EstimModeSelectEnumerationValues AttitudeEstimationMode : 8;
};
static_assert(sizeof(SetAttitudeEstimationModeCommandFormat) == 1);
template <> struct MessageSize<SetAttitudeEstimationModeCommandFormat> { static constexpr size_t value = 1; };
template <> struct SetID<SetAttitudeEstimationModeCommandFormat> { static constexpr int value = 14; };

struct SetMagnetorquerOutputCommandFormat {
  int16_t CommandedXMagnetorquerdutycycle;
  int16_t CommandedYMagnetorquerdutycycle;
  int16_t CommandedZMagnetorquerdutycycle;
};
static_assert(sizeof(SetMagnetorquerOutputCommandFormat) == 6);
template <> struct MessageSize<SetMagnetorquerOutputCommandFormat> { static constexpr size_t value = 6; };
template <> struct SetID<SetMagnetorquerOutputCommandFormat> { static constexpr int value = 16; };

struct SetWheelSpeedCommandFormat {
  int16_t CommandedXspeed;
  int16_t CommandedYspeed;
  int16_t CommandedZspeed;
};
static_assert(sizeof(SetWheelSpeedCommandFormat) == 6);
template <> struct MessageSize<SetWheelSpeedCommandFormat> { static constexpr size_t value = 6; };
template <> struct SetID<SetWheelSpeedCommandFormat> { static constexpr int value = 17; };

struct TriggerADCSLoopCommandFormat {
};
static_assert(sizeof(TriggerADCSLoopCommandFormat) == 1);
template <> struct MessageSize<TriggerADCSLoopCommandFormat> { static constexpr size_t value = 0; };
template <> struct SetID<TriggerADCSLoopCommandFormat> { static constexpr int value = 18; };

struct TriggerADCSLoopwithSimulatedSensorDataCommandFormat {
  uint32_t UnixTime;
  uint16_t CssRaw1;
  uint16_t CssRaw2;
  uint16_t CssRaw3;
  uint16_t CssRaw4;
  uint16_t CssRaw5;
  uint16_t CssRaw6;
  uint16_t CssRaw7;
  uint16_t CssRaw8;
  uint16_t CssRaw9;
  uint16_t CssRaw10;
  int16_t Cam1RawX;
  int16_t Cam1RawY;
  uint8_t Cam1Busy;
  uint8_t Cam1Result;
  int16_t Cam2RawX;
  int16_t Cam2RawY;
  uint8_t Cam2Busy;
  uint8_t Cam2Result;
  int16_t MagRawX;
  int16_t MagRawY;
  int16_t MagRawZ;
  int32_t RateRawX;
  int32_t RateRawY;
  int32_t RateRawZ;
  int16_t WheelRawX;
  int16_t WheelRawY;
  int16_t WheelRawZ;
  int16_t Star1CameraX;
  int16_t Star1CameraY;
  int16_t Star1CameraZ;
  int16_t Star1InertialX;
  int16_t Star1InertialY;
  int16_t Star1InertialZ;
  int16_t Star2CameraX;
  int16_t Star2CameraY;
  int16_t Star2CameraZ;
  int16_t Star2InertialX;
  int16_t Star2InertialY;
  int16_t Star2InertialZ;
  int16_t Star3CameraX;
  int16_t Star3CameraY;
  int16_t Star3CameraZ;
  int16_t Star3InertialX;
  int16_t Star3InertialY;
  int16_t Star3InertialZ;
  GpsSolutionStatusEnumerationValues GpsSolutionStatus : 8;
  uint16_t GPSReferenceWeek;
  uint32_t GPSTimeMilliseconds;
  int32_t ECEFPositionX;
  int16_t ECEFVelocityX;
  int32_t ECEFPositionY;
  int16_t ECEFVelocityY;
  int32_t ECEFPositionZ;
  int16_t ECEFVelocityZ;
  uint8_t XposStandardDeviation;
  uint8_t YposStandardDeviation;
  uint8_t ZposStandardDeviation;
  uint8_t XvelStandardDeviation;
  uint8_t YvelStandardDeviation;
  uint8_t ZvelStandardDeviation;
};
static_assert(sizeof(TriggerADCSLoopwithSimulatedSensorDataCommandFormat) == 127);
template <> struct MessageSize<TriggerADCSLoopwithSimulatedSensorDataCommandFormat> { static constexpr size_t value = 127; };
template <> struct SetID<TriggerADCSLoopwithSimulatedSensorDataCommandFormat> { static constexpr int value = 19; };

struct ASGP4RunModeCommandFormat {
  Asgp4ModeSelectEnumerationValues ASGP4Mode : 8;
};
static_assert(sizeof(ASGP4RunModeCommandFormat) == 1);
template <> struct MessageSize<ASGP4RunModeCommandFormat> { static constexpr size_t value = 1; };
template <> struct SetID<ASGP4RunModeCommandFormat> { static constexpr int value = 31; };

struct ASGP4TriggerCommandFormat {
};
static_assert(sizeof(ASGP4TriggerCommandFormat) == 1);
template <> struct MessageSize<ASGP4TriggerCommandFormat> { static constexpr size_t value = 0; };
template <> struct SetID<ASGP4TriggerCommandFormat> { static constexpr int value = 32; };

struct SetModeofMagnetometerOperationCommandFormat {
  MagModeValEnumerationValues MagnetometerMode : 8;
};
static_assert(sizeof(SetModeofMagnetometerOperationCommandFormat) == 1);
template <> struct MessageSize<SetModeofMagnetometerOperationCommandFormat> { static constexpr size_t value = 1; };
template <> struct SetID<SetModeofMagnetometerOperationCommandFormat> { static constexpr int value = 56; };

struct ConverttoJPGfileCommandFormat {
  uint8_t SourceFileCounter;
  uint8_t QualityFactor;
  uint8_t WhiteBalance;
};
static_assert(sizeof(ConverttoJPGfileCommandFormat) == 3);
template <> struct MessageSize<ConverttoJPGfileCommandFormat> { static constexpr size_t value = 3; };
template <> struct SetID<ConverttoJPGfileCommandFormat> { static constexpr int value = 57; };

struct SaveConfigurationCommandFormat {
};
static_assert(sizeof(SaveConfigurationCommandFormat) == 1);
template <> struct MessageSize<SaveConfigurationCommandFormat> { static constexpr size_t value = 0; };
template <> struct SetID<SaveConfigurationCommandFormat> { static constexpr int value = 63; };

struct SaveOrbitParametersCommandFormat {
};
static_assert(sizeof(SaveOrbitParametersCommandFormat) == 1);
template <> struct MessageSize<SaveOrbitParametersCommandFormat> { static constexpr size_t value = 0; };
template <> struct SetID<SaveOrbitParametersCommandFormat> { static constexpr int value = 64; };

struct SaveImageCommandFormat {
  CamSelectEnumerationValues CameraSelect : 8;
  ImSizeEnumerationValues ImageSize : 8;
};
static_assert(sizeof(SaveImageCommandFormat) == 2);
template <> struct MessageSize<SaveImageCommandFormat> { static constexpr size_t value = 2; };
template <> struct SetID<SaveImageCommandFormat> { static constexpr int value = 80; };

struct CurrentADCSStateTelemetryFormat {
  EstimModeSelectEnumerationValues AttitudeEstimationMode : 4;
  ConModeSelectEnumerationValues ControlMode : 4;
  AdcsRunModeEnumerationValues ADCSRunMode : 2;
  Asgp4ModeSelectEnumerationValues ASGP4Mode : 2;
  bool CubeControlSignalEnabled : 1;
  bool CubeControlMotorEnabled : 1;
  bool CubeSense1Enabled : 1;
  bool CubeSense2Enabled : 1;
  bool CubeWheel1Enabled : 1;
  bool CubeWheel2Enabled : 1;
  bool CubeWheel3Enabled : 1;
  bool CubeStarEnabled : 1;
  bool GPSReceiverEnabled : 1;
  bool GPSLNAPowerEnabled : 1;
  bool MotorDriverEnabled : 1;
  bool SunisAboveLocalHorizon : 1;
  bool CubeSense1CommunicationsError : 1;
  bool CubeSense2CommunicationsError : 1;
  bool CubeControlSignalCommunicationsError : 1;
  bool CubeControlMotorCommunicationsError : 1;
  bool CubeWheel1CommunicationsError : 1;
  bool CubeWheel2CommunicationsError : 1;
  bool CubeWheel3CommunicationsError : 1;
  bool CubeStarCommunicationsError : 1;
  bool MagnetometerRangeError : 1;
  bool Cam1SRAMOvercurrentDetected : 1;
  bool Cam13V3OvercurrentDetected : 1;
  bool Cam1SensorBusyError : 1;
  bool Cam1SensorDetectionError : 1;
  bool SunSensorRangeError : 1;
  bool Cam2SRAMOvercurrentDetected : 1;
  bool Cam23V3OvercurrentDetected : 1;
  bool Cam2SensorBusyError : 1;
  bool Cam2SensorDetectionError : 1;
  bool NadirSensorRangeError : 1;
  bool RateSensorRangeError : 1;
  bool WheelSpeedRangeError : 1;
  bool CoarseSunSensorError : 1;
  bool StarTrackerMatchError : 1;
  bool StarTrackerOvercurrentDetected : 1;
};
static_assert(sizeof(CurrentADCSStateTelemetryFormat) == 6);
template <> struct MessageSize<CurrentADCSStateTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<CurrentADCSStateTelemetryFormat> { static constexpr int value = 132; };

struct JPGConversionProgressTelemetryFormat {
  uint8_t ProgressPercentage;
  JpgConvertResultEnumerationValues ConversionResult : 8;
  uint8_t OutputFileCounter;
};
static_assert(sizeof(JPGConversionProgressTelemetryFormat) == 3);
template <> struct MessageSize<JPGConversionProgressTelemetryFormat> { static constexpr size_t value = 3; };
template <> struct GetID<JPGConversionProgressTelemetryFormat> { static constexpr int value = 133; };

struct CubeACPStateTelemetryFormat {
  bool ADCSConfigLoadError : 1;
  bool OrbitParameterLoadError : 1;
  bool SystemConfigurationLoadError : 1;
  bool SDcardinitialisationerror : 1;
  bool SDcardreaderror : 1;
  bool SDcardwriteerror : 1;
};
static_assert(sizeof(CubeACPStateTelemetryFormat) == 1);
template <> struct MessageSize<CubeACPStateTelemetryFormat> { static constexpr size_t value = 1; };
template <> struct GetID<CubeACPStateTelemetryFormat> { static constexpr int value = 135; };

struct EstimatedAttitudeAnglesTelemetryFormat {
  int16_t EstimatedRollAngle;
  int16_t EstimatedPitchAngle;
  int16_t EstimatedYawAngle;
};
static_assert(sizeof(EstimatedAttitudeAnglesTelemetryFormat) == 6);
template <> struct MessageSize<EstimatedAttitudeAnglesTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<EstimatedAttitudeAnglesTelemetryFormat> { static constexpr int value = 146; };

struct EstimatedAngularRatesTelemetryFormat {
  int16_t EstimatedXAngularRate;
  int16_t EstimatedYAngularRate;
  int16_t EstimatedZAngularRate;
};
static_assert(sizeof(EstimatedAngularRatesTelemetryFormat) == 6);
template <> struct MessageSize<EstimatedAngularRatesTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<EstimatedAngularRatesTelemetryFormat> { static constexpr int value = 147; };

struct SatellitePositionECITelemetryFormat {
  int16_t Xposition;
  int16_t Yposition;
  int16_t Zposition;
};
static_assert(sizeof(SatellitePositionECITelemetryFormat) == 6);
template <> struct MessageSize<SatellitePositionECITelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<SatellitePositionECITelemetryFormat> { static constexpr int value = 148; };

struct SatelliteVelocityECITelemetryFormat {
  int16_t XVelocity;
  int16_t YVelocity;
  int16_t ZVelocity;
};
static_assert(sizeof(SatelliteVelocityECITelemetryFormat) == 6);
template <> struct MessageSize<SatelliteVelocityECITelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<SatelliteVelocityECITelemetryFormat> { static constexpr int value = 149; };

struct SatellitePositionLLHTelemetryFormat {
  int16_t Latitude;
  int16_t Longitude;
  uint16_t Altitude;
};
static_assert(sizeof(SatellitePositionLLHTelemetryFormat) == 6);
template <> struct MessageSize<SatellitePositionLLHTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<SatellitePositionLLHTelemetryFormat> { static constexpr int value = 150; };

struct MagneticFieldVectorTelemetryFormat {
  int16_t MagneticFieldX;
  int16_t MagneticFieldY;
  int16_t MagneticFieldZ;
};
static_assert(sizeof(MagneticFieldVectorTelemetryFormat) == 6);
template <> struct MessageSize<MagneticFieldVectorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<MagneticFieldVectorTelemetryFormat> { static constexpr int value = 151; };

struct CoarseSunVectorTelemetryFormat {
  int16_t CoarseSunX;
  int16_t CoarseSunY;
  int16_t CoarseSunZ;
};
static_assert(sizeof(CoarseSunVectorTelemetryFormat) == 6);
template <> struct MessageSize<CoarseSunVectorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<CoarseSunVectorTelemetryFormat> { static constexpr int value = 152; };

struct FineSunVectorTelemetryFormat {
  int16_t SunX;
  int16_t SunY;
  int16_t SunZ;
};
static_assert(sizeof(FineSunVectorTelemetryFormat) == 6);
template <> struct MessageSize<FineSunVectorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<FineSunVectorTelemetryFormat> { static constexpr int value = 153; };

struct NadirVectorTelemetryFormat {
  int16_t NadirX;
  int16_t NadirY;
  int16_t NadirZ;
};
static_assert(sizeof(NadirVectorTelemetryFormat) == 6);
template <> struct MessageSize<NadirVectorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<NadirVectorTelemetryFormat> { static constexpr int value = 154; };

struct RateSensorRatesTelemetryFormat {
  int16_t XAngularRate;
  int16_t YAngularRate;
  int16_t ZAngularRate;
};
static_assert(sizeof(RateSensorRatesTelemetryFormat) == 6);
template <> struct MessageSize<RateSensorRatesTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<RateSensorRatesTelemetryFormat> { static constexpr int value = 155; };

struct WheelSpeedTelemetryFormat {
  int16_t XWheelSpeed;
  int16_t YWheelSpeed;
  int16_t ZWheelSpeed;
};
static_assert(sizeof(WheelSpeedTelemetryFormat) == 6);
template <> struct MessageSize<WheelSpeedTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<WheelSpeedTelemetryFormat> { static constexpr int value = 156; };

struct MagnetorquerCommandTelemetryFormat {
  int16_t XMagnetorquerCommand;
  int16_t YMagnetorquerCommand;
  int16_t ZMagnetorquerCommand;
};
static_assert(sizeof(MagnetorquerCommandTelemetryFormat) == 6);
template <> struct MessageSize<MagnetorquerCommandTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<MagnetorquerCommandTelemetryFormat> { static constexpr int value = 157; };

struct WheelSpeedCommandsTelemetryFormat {
  int16_t CommandedXWheelSpeed;
  int16_t CommandedYWheelSpeed;
  int16_t CommandedZWheelSpeed;
};
static_assert(sizeof(WheelSpeedCommandsTelemetryFormat) == 6);
template <> struct MessageSize<WheelSpeedCommandsTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<WheelSpeedCommandsTelemetryFormat> { static constexpr int value = 158; };

struct IGRFModelledMagneticFieldVectorTelemetryFormat {
  int16_t IGRFModelledMagneticFieldX;
  int16_t IGRFModelledMagneticFieldY;
  int16_t IGRFModelledMagneticFieldZ;
};
static_assert(sizeof(IGRFModelledMagneticFieldVectorTelemetryFormat) == 6);
template <> struct MessageSize<IGRFModelledMagneticFieldVectorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<IGRFModelledMagneticFieldVectorTelemetryFormat> { static constexpr int value = 159; };

struct ModelledSunVectorTelemetryFormat {
  int16_t ModelledSunVectorX;
  int16_t ModelledSunVectorY;
  int16_t ModelledSunVectorZ;
};
static_assert(sizeof(ModelledSunVectorTelemetryFormat) == 6);
template <> struct MessageSize<ModelledSunVectorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<ModelledSunVectorTelemetryFormat> { static constexpr int value = 160; };

struct EstimatedGyroBiasTelemetryFormat {
  int16_t EstimatedXgyroBias;
  int16_t EstimatedYgyroBias;
  int16_t EstimatedZgyroBias;
};
static_assert(sizeof(EstimatedGyroBiasTelemetryFormat) == 6);
template <> struct MessageSize<EstimatedGyroBiasTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<EstimatedGyroBiasTelemetryFormat> { static constexpr int value = 161; };

struct EstimationInnovationVectorTelemetryFormat {
  int16_t InnovationVectorX;
  int16_t InnovationVectorY;
  int16_t InnovationVectorZ;
};
static_assert(sizeof(EstimationInnovationVectorTelemetryFormat) == 6);
template <> struct MessageSize<EstimationInnovationVectorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<EstimationInnovationVectorTelemetryFormat> { static constexpr int value = 162; };

struct QuaternionErrorVectorTelemetryFormat {
  int16_t QuaternionErrorQ1;
  int16_t QuaternionErrorQ2;
  int16_t QuaternionErrorQ3;
};
static_assert(sizeof(QuaternionErrorVectorTelemetryFormat) == 6);
template <> struct MessageSize<QuaternionErrorVectorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<QuaternionErrorVectorTelemetryFormat> { static constexpr int value = 163; };

struct QuaternionCovarianceTelemetryFormat {
  int16_t QuaternionCovarianceQ1RMS;
  int16_t QuaternionCovarianceQ2RMS;
  int16_t QuaternionCovarianceQ3RMS;
};
static_assert(sizeof(QuaternionCovarianceTelemetryFormat) == 6);
template <> struct MessageSize<QuaternionCovarianceTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<QuaternionCovarianceTelemetryFormat> { static constexpr int value = 164; };

struct AngularRateCovarianceTelemetryFormat {
  int16_t XAngularRateCovariance;
  int16_t YAngularRateCovariance;
  int16_t ZAngularRateCovariance;
};
static_assert(sizeof(AngularRateCovarianceTelemetryFormat) == 6);
template <> struct MessageSize<AngularRateCovarianceTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<AngularRateCovarianceTelemetryFormat> { static constexpr int value = 165; };

struct RawCam2SensorTelemetryFormat {
  int16_t Cam2centroidX;
  int16_t Cam2centroidY;
  CaptureResultEnumerationValues Cam2Capturestatus : 8;
  DetectResultEnumerationValues Cam2Detectionresult : 8;
};
static_assert(sizeof(RawCam2SensorTelemetryFormat) == 6);
template <> struct MessageSize<RawCam2SensorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<RawCam2SensorTelemetryFormat> { static constexpr int value = 166; };

struct RawCam1SensorTelemetryFormat {
  int16_t Cam1centroidX;
  int16_t Cam1centroidY;
  CaptureResultEnumerationValues Cam1Capturestatus : 8;
  DetectResultEnumerationValues Cam1Detectionresult : 8;
};
static_assert(sizeof(RawCam1SensorTelemetryFormat) == 6);
template <> struct MessageSize<RawCam1SensorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<RawCam1SensorTelemetryFormat> { static constexpr int value = 167; };

struct RawCSS1to6TelemetryFormat {
  uint8_t CSS1;
  uint8_t CSS2;
  uint8_t CSS3;
  uint8_t CSS4;
  uint8_t CSS5;
  uint8_t CSS6;
};
static_assert(sizeof(RawCSS1to6TelemetryFormat) == 6);
template <> struct MessageSize<RawCSS1to6TelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<RawCSS1to6TelemetryFormat> { static constexpr int value = 168; };

struct RawCSS7to10TelemetryFormat {
  uint8_t CSS7;
  uint8_t CSS8;
  uint8_t CSS9;
  uint8_t CSS10;
  char unused_0[2];
};
static_assert(sizeof(RawCSS7to10TelemetryFormat) == 6);
template <> struct MessageSize<RawCSS7to10TelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<RawCSS7to10TelemetryFormat> { static constexpr int value = 169; };

struct RawMagnetometerTelemetryFormat {
  int16_t MagX;
  int16_t MagY;
  int16_t MagZ;
};
static_assert(sizeof(RawMagnetometerTelemetryFormat) == 6);
template <> struct MessageSize<RawMagnetometerTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<RawMagnetometerTelemetryFormat> { static constexpr int value = 170; };

struct CubeSense1CurrentMeasurementsTelemetryFormat {
  uint16_t CubeSense13V3Current;
  uint16_t CubeSense1CamSRAMCurrent;
};
static_assert(sizeof(CubeSense1CurrentMeasurementsTelemetryFormat) == 4);
template <> struct MessageSize<CubeSense1CurrentMeasurementsTelemetryFormat> { static constexpr size_t value = 4; };
template <> struct GetID<CubeSense1CurrentMeasurementsTelemetryFormat> { static constexpr int value = 171; };

struct CubeControlCurrentMeasurementsTelemetryFormat {
  uint16_t CubeControl3V3Current;
  uint16_t CubeControl5VCurrent;
  uint16_t CubeControlVbatCurrent;
};
static_assert(sizeof(CubeControlCurrentMeasurementsTelemetryFormat) == 6);
template <> struct MessageSize<CubeControlCurrentMeasurementsTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<CubeControlCurrentMeasurementsTelemetryFormat> { static constexpr int value = 172; };

struct WheelCurrentsTelemetryFormat {
  uint16_t Wheel1Current;
  uint16_t Wheel2Current;
  uint16_t Wheel3Current;
};
static_assert(sizeof(WheelCurrentsTelemetryFormat) == 6);
template <> struct MessageSize<WheelCurrentsTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<WheelCurrentsTelemetryFormat> { static constexpr int value = 173; };

struct ADCSTemperaturesTelemetryFormat {
  int16_t MCUTemperature;
  int16_t MagnetometerTemperature;
  int16_t RedundantMagnetometerTemperature;
};
static_assert(sizeof(ADCSTemperaturesTelemetryFormat) == 6);
template <> struct MessageSize<ADCSTemperaturesTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<ADCSTemperaturesTelemetryFormat> { static constexpr int value = 174; };

struct RatesensortemperaturesTelemetryFormat {
  int16_t XRateSensorTemperature;
  int16_t YRateSensorTemperature;
  int16_t ZRateSensorTemperature;
};
static_assert(sizeof(RatesensortemperaturesTelemetryFormat) == 6);
template <> struct MessageSize<RatesensortemperaturesTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<RatesensortemperaturesTelemetryFormat> { static constexpr int value = 175; };

struct RawGPSStatusTelemetryFormat {
  GpsSolutionStatusEnumerationValues GpsSolutionStatus : 8;
  uint8_t NumberoftrackedGPSsatellites;
  uint8_t NumberofGPSsatellitesusedinsolution;
  uint8_t CounterforXYZLoffromGPS;
  uint8_t CounterforRANGElogfromGPS;
  uint8_t ResponseMessageforGPSlogsetup;
};
static_assert(sizeof(RawGPSStatusTelemetryFormat) == 6);
template <> struct MessageSize<RawGPSStatusTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<RawGPSStatusTelemetryFormat> { static constexpr int value = 176; };

struct RawGPSTimeTelemetryFormat {
  uint16_t GPSReferenceWeek;
  uint32_t GPSTimeMilliseconds;
};
static_assert(sizeof(RawGPSTimeTelemetryFormat) == 6);
template <> struct MessageSize<RawGPSTimeTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<RawGPSTimeTelemetryFormat> { static constexpr int value = 177; };

struct RawGPSXTelemetryFormat {
  int32_t ECEFPositionX;
  int16_t ECEFVelocityX;
};
static_assert(sizeof(RawGPSXTelemetryFormat) == 6);
template <> struct MessageSize<RawGPSXTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<RawGPSXTelemetryFormat> { static constexpr int value = 178; };

struct RawGPSYTelemetryFormat {
  int32_t ECEFPositionY;
  int16_t ECEFVelocityY;
};
static_assert(sizeof(RawGPSYTelemetryFormat) == 6);
template <> struct MessageSize<RawGPSYTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<RawGPSYTelemetryFormat> { static constexpr int value = 179; };

struct RawGPSZTelemetryFormat {
  int32_t ECEFPositionZ;
  int16_t ECEFVelocityZ;
};
static_assert(sizeof(RawGPSZTelemetryFormat) == 6);
template <> struct MessageSize<RawGPSZTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<RawGPSZTelemetryFormat> { static constexpr int value = 180; };

struct Star1BodyVectorTelemetryFormat {
  int16_t Star1BX;
  int16_t Star1BY;
  int16_t Star1BZ;
};
static_assert(sizeof(Star1BodyVectorTelemetryFormat) == 6);
template <> struct MessageSize<Star1BodyVectorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<Star1BodyVectorTelemetryFormat> { static constexpr int value = 181; };

struct Star2BodyVectorTelemetryFormat {
  int16_t Star2BX;
  int16_t Star2BY;
  int16_t Star2BZ;
};
static_assert(sizeof(Star2BodyVectorTelemetryFormat) == 6);
template <> struct MessageSize<Star2BodyVectorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<Star2BodyVectorTelemetryFormat> { static constexpr int value = 182; };

struct Star3BodyVectorTelemetryFormat {
  int16_t Star3BX;
  int16_t Star3BY;
  int16_t Star3BZ;
};
static_assert(sizeof(Star3BodyVectorTelemetryFormat) == 6);
template <> struct MessageSize<Star3BodyVectorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<Star3BodyVectorTelemetryFormat> { static constexpr int value = 183; };

struct Star1OrbitVectorTelemetryFormat {
  int16_t Star1OX;
  int16_t Star1OY;
  int16_t Star1OZ;
};
static_assert(sizeof(Star1OrbitVectorTelemetryFormat) == 6);
template <> struct MessageSize<Star1OrbitVectorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<Star1OrbitVectorTelemetryFormat> { static constexpr int value = 184; };

struct Star2OrbitVectorTelemetryFormat {
  int16_t Star2OX;
  int16_t Star2OY;
  int16_t Star2OZ;
};
static_assert(sizeof(Star2OrbitVectorTelemetryFormat) == 6);
template <> struct MessageSize<Star2OrbitVectorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<Star2OrbitVectorTelemetryFormat> { static constexpr int value = 185; };

struct Star3OrbitVectorTelemetryFormat {
  int16_t Star3OX;
  int16_t Star3OY;
  int16_t Star3OZ;
};
static_assert(sizeof(Star3OrbitVectorTelemetryFormat) == 6);
template <> struct MessageSize<Star3OrbitVectorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<Star3OrbitVectorTelemetryFormat> { static constexpr int value = 186; };

struct StarMagnitudeTelemetryFormat {
  uint16_t MagnitudeStar1;
  uint16_t MagnitudeStar2;
  uint16_t MagnitudeStar3;
};
static_assert(sizeof(StarMagnitudeTelemetryFormat) == 6);
template <> struct MessageSize<StarMagnitudeTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<StarMagnitudeTelemetryFormat> { static constexpr int value = 187; };

struct StarPerformance1TelemetryFormat {
  uint8_t Numberofstarsdetected;
  uint8_t Starimagenoise;
  uint8_t InvalidStars;
  uint8_t Numberofstarsidentified;
  StarIDModeValEnumerationValues Identificationmode : 8;
  uint8_t Imagedarkvalue;
};
static_assert(sizeof(StarPerformance1TelemetryFormat) == 6);
template <> struct MessageSize<StarPerformance1TelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<StarPerformance1TelemetryFormat> { static constexpr int value = 188; };

struct StarTimingTelemetryFormat {
  uint16_t Capture;
  uint16_t Detection;
  uint16_t Identification;
};
static_assert(sizeof(StarTimingTelemetryFormat) == 6);
template <> struct MessageSize<StarTimingTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<StarTimingTelemetryFormat> { static constexpr int value = 189; };

struct ADCSStateTelemetryFormat {
  EstimModeSelectEnumerationValues AttitudeEstimationMode : 4;
  ConModeSelectEnumerationValues ControlMode : 4;
  AdcsRunModeEnumerationValues ADCSRunMode : 2;
  Asgp4ModeSelectEnumerationValues ASGP4Mode : 2;
  bool CubeControlSignalEnabled : 1;
  bool CubeControlMotorEnabled : 1;
  bool CubeSense1Enabled : 1;
  bool CubeSense2Enabled : 1;
  bool CubeWheel1Enabled : 1;
  bool CubeWheel2Enabled : 1;
  bool CubeWheel3Enabled : 1;
  bool CubeStarEnabled : 1;
  bool GPSReceiverEnabled : 1;
  bool GPSLNAPowerEnabled : 1;
  bool MotorDriverEnabled : 1;
  bool SunisAboveLocalHorizon : 1;
  bool CubeSense1CommunicationsError : 1;
  bool CubeSense2CommunicationsError : 1;
  bool CubeControlSignalCommunicationsError : 1;
  bool CubeControlMotorCommunicationsError : 1;
  bool CubeWheel1CommunicationsError : 1;
  bool CubeWheel2CommunicationsError : 1;
  bool CubeWheel3CommunicationsError : 1;
  bool CubeStarCommunicationsError : 1;
  bool MagnetometerRangeError : 1;
  bool Cam1SRAMOvercurrentDetected : 1;
  bool Cam13V3OvercurrentDetected : 1;
  bool Cam1SensorBusyError : 1;
  bool Cam1SensorDetectionError : 1;
  bool SunSensorRangeError : 1;
  bool Cam2SRAMOvercurrentDetected : 1;
  bool Cam23V3OvercurrentDetected : 1;
  bool Cam2SensorBusyError : 1;
  bool Cam2SensorDetectionError : 1;
  bool NadirSensorRangeError : 1;
  bool RateSensorRangeError : 1;
  bool WheelSpeedRangeError : 1;
  bool CoarseSunSensorError : 1;
  bool StarTrackerMatchError : 1;
  bool StarTrackerOvercurrentDetected : 1;
  bool OrbitParametersareInvalid : 1;
  bool ConfigurationisInvalid : 1;
  bool ControlModeChangeisnotallowed : 1;
  bool EstimatorChangeisnotallowed : 1;
  MagModeValEnumerationValues CurrentMagnetometerSamplingMode : 2;
  bool Modelledandmeasuredmagneticfielddiffersinsize : 1;
  bool NodeRecoveryError : 1;
  bool CubeSense1RuntimeError : 1;
  bool CubeSense2RuntimeError : 1;
  bool CubeControlSignalRuntimeError : 1;
  bool CubeControlMotorRuntimeError : 1;
  bool CubeWheel1RuntimeError : 1;
  bool CubeWheel2RuntimeError : 1;
  bool CubeWheel3RuntimeError : 1;
  bool CubeStarRuntimeError : 1;
  bool MagnetometerError : 1;
  bool RateSensorFailure : 1;
  char unused_0[3];
  int16_t EstimatedRollAngle;
  int16_t EstimatedPitchAngle;
  int16_t EstimatedYawAngle;
  int16_t Estimatedq1;
  int16_t Estimatedq2;
  int16_t Estimatedq3;
  int16_t EstimatedXAngularRate;
  int16_t EstimatedYAngularRate;
  int16_t EstimatedZAngularRate;
  int16_t Xposition;
  int16_t Yposition;
  int16_t Zposition;
  int16_t XVelocity;
  int16_t YVelocity;
  int16_t ZVelocity;
  int16_t Latitude;
  int16_t Longitude;
  uint16_t Altitude;
  int16_t ECEFPositionX;
  int16_t ECEFPositionY;
  int16_t ECEFPositionZ;
};
static_assert(sizeof(ADCSStateTelemetryFormat) == 54);
template <> struct MessageSize<ADCSStateTelemetryFormat> { static constexpr size_t value = 54; };
template <> struct GetID<ADCSStateTelemetryFormat> { static constexpr int value = 190; };

struct ADCSMeasurementsTelemetryFormat {
  int16_t MagneticFieldX;
  int16_t MagneticFieldY;
  int16_t MagneticFieldZ;
  int16_t CoarseSunX;
  int16_t CoarseSunY;
  int16_t CoarseSunZ;
  int16_t SunX;
  int16_t SunY;
  int16_t SunZ;
  int16_t NadirX;
  int16_t NadirY;
  int16_t NadirZ;
  int16_t XAngularRate;
  int16_t YAngularRate;
  int16_t ZAngularRate;
  int16_t XWheelSpeed;
  int16_t YWheelSpeed;
  int16_t ZWheelSpeed;
  int16_t Star1BX;
  int16_t Star1BY;
  int16_t Star1BZ;
  int16_t Star1OX;
  int16_t Star1OY;
  int16_t Star1OZ;
  int16_t Star2BX;
  int16_t Star2BY;
  int16_t Star2BZ;
  int16_t Star2OX;
  int16_t Star2OY;
  int16_t Star2OZ;
  int16_t Star3BX;
  int16_t Star3BY;
  int16_t Star3BZ;
  int16_t Star3OX;
  int16_t Star3OY;
  int16_t Star3OZ;
};
static_assert(sizeof(ADCSMeasurementsTelemetryFormat) == 72);
template <> struct MessageSize<ADCSMeasurementsTelemetryFormat> { static constexpr size_t value = 72; };
template <> struct GetID<ADCSMeasurementsTelemetryFormat> { static constexpr int value = 191; };

struct ActuatorCommandsTelemetryFormat {
  int16_t XMagnetorquerCommand;
  int16_t YMagnetorquerCommand;
  int16_t ZMagnetorquerCommand;
  int16_t CommandedXWheelSpeed;
  int16_t CommandedYWheelSpeed;
  int16_t CommandedZWheelSpeed;
};
static_assert(sizeof(ActuatorCommandsTelemetryFormat) == 12);
template <> struct MessageSize<ActuatorCommandsTelemetryFormat> { static constexpr size_t value = 12; };
template <> struct GetID<ActuatorCommandsTelemetryFormat> { static constexpr int value = 192; };

struct EstimationDataTelemetryFormat {
  int16_t IGRFModelledMagneticFieldX;
  int16_t IGRFModelledMagneticFieldY;
  int16_t IGRFModelledMagneticFieldZ;
  int16_t ModelledSunVectorX;
  int16_t ModelledSunVectorY;
  int16_t ModelledSunVectorZ;
  int16_t EstimatedXgyroBias;
  int16_t EstimatedYgyroBias;
  int16_t EstimatedZgyroBias;
  int16_t InnovationVectorX;
  int16_t InnovationVectorY;
  int16_t InnovationVectorZ;
  int16_t QuaternionErrorQ1;
  int16_t QuaternionErrorQ2;
  int16_t QuaternionErrorQ3;
  int16_t QuaternionCovarianceQ1RMS;
  int16_t QuaternionCovarianceQ2RMS;
  int16_t QuaternionCovarianceQ3RMS;
  int16_t XAngularRateCovariance;
  int16_t YAngularRateCovariance;
  int16_t ZAngularRateCovariance;
};
static_assert(sizeof(EstimationDataTelemetryFormat) == 42);
template <> struct MessageSize<EstimationDataTelemetryFormat> { static constexpr size_t value = 42; };
template <> struct GetID<EstimationDataTelemetryFormat> { static constexpr int value = 193; };

struct RawSensorMeasurementsTelemetryFormat {
  int16_t Cam2centroidX;
  int16_t Cam2centroidY;
  CaptureResultEnumerationValues Cam2Capturestatus : 8;
  DetectResultEnumerationValues Cam2Detectionresult : 8;
  int16_t Cam1centroidX;
  int16_t Cam1centroidY;
  CaptureResultEnumerationValues Cam1Capturestatus : 8;
  DetectResultEnumerationValues Cam1Detectionresult : 8;
  uint8_t CSS1;
  uint8_t CSS2;
  uint8_t CSS3;
  uint8_t CSS4;
  uint8_t CSS5;
  uint8_t CSS6;
  uint8_t CSS7;
  uint8_t CSS8;
  uint8_t CSS9;
  uint8_t CSS10;
  int16_t MagX;
  int16_t MagY;
  int16_t MagZ;
  int16_t RateX;
  int16_t RateY;
  int16_t RateZ;
};
static_assert(sizeof(RawSensorMeasurementsTelemetryFormat) == 34);
template <> struct MessageSize<RawSensorMeasurementsTelemetryFormat> { static constexpr size_t value = 34; };
template <> struct GetID<RawSensorMeasurementsTelemetryFormat> { static constexpr int value = 194; };

struct PowerandTemperatureMeasurementsTelemetryFormat {
  uint16_t CubeSense13V3Current;
  uint16_t CubeSense1CamSRAMCurrent;
  uint16_t CubeSense23V3Current;
  uint16_t CubeSense2CamSRAMCurrent;
  uint16_t CubeControl3V3Current;
  uint16_t CubeControl5VCurrent;
  uint16_t CubeControlVbatCurrent;
  uint16_t Wheel1Current;
  uint16_t Wheel2Current;
  uint16_t Wheel3Current;
  uint16_t CubeStarCurrent;
  uint16_t MagnetorquerCurrent;
  int16_t CubeStarMCUtemperature;
  int16_t MCUTemperature;
  int16_t MagnetometerTemperature;
  int16_t RedundantMagnetometerTemperature;
  int16_t XRateSensorTemperature;
  int16_t YRateSensorTemperature;
  int16_t ZRateSensorTemperature;
};
static_assert(sizeof(PowerandTemperatureMeasurementsTelemetryFormat) == 38);
template <> struct MessageSize<PowerandTemperatureMeasurementsTelemetryFormat> { static constexpr size_t value = 38; };
template <> struct GetID<PowerandTemperatureMeasurementsTelemetryFormat> { static constexpr int value = 195; };

struct AdcsExecutionTimesTelemetryFormat {
  uint16_t TimetoPerformADCSUpdate;
  uint16_t TimetoPerformSensorActuatorCommunications;
  uint16_t TimetoExecuteSGP4Propagator;
  uint16_t TimetoExecuteIGRFModel;
};
static_assert(sizeof(AdcsExecutionTimesTelemetryFormat) == 8);
template <> struct MessageSize<AdcsExecutionTimesTelemetryFormat> { static constexpr size_t value = 8; };
template <> struct GetID<AdcsExecutionTimesTelemetryFormat> { static constexpr int value = 196; };

struct ADCSMiscCurrentMeasurementsTelemetryFormat {
  uint16_t CubeStarCurrent;
  uint16_t MagnetorquerCurrent;
  int16_t CubeStarMCUtemperature;
};
static_assert(sizeof(ADCSMiscCurrentMeasurementsTelemetryFormat) == 6);
template <> struct MessageSize<ADCSMiscCurrentMeasurementsTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<ADCSMiscCurrentMeasurementsTelemetryFormat> { static constexpr int value = 198; };

struct FineEstimatedAngularRatesTelemetryFormat {
  int16_t EstimatedXAngularRate;
  int16_t EstimatedYAngularRate;
  int16_t EstimatedZAngularRate;
};
static_assert(sizeof(FineEstimatedAngularRatesTelemetryFormat) == 6);
template <> struct MessageSize<FineEstimatedAngularRatesTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<FineEstimatedAngularRatesTelemetryFormat> { static constexpr int value = 201; };

struct RawGPSMeasurementsTelemetryFormat {
  GpsSolutionStatusEnumerationValues GpsSolutionStatus : 8;
  uint8_t NumberoftrackedGPSsatellites;
  uint8_t NumberofGPSsatellitesusedinsolution;
  uint8_t CounterforXYZLoffromGPS;
  uint8_t CounterforRANGElogfromGPS;
  uint8_t ResponseMessageforGPSlogsetup;
  uint16_t GPSReferenceWeek;
  uint32_t GPSTimeMilliseconds;
  int32_t ECEFPositionX;
  int16_t ECEFVelocityX;
  int32_t ECEFPositionY;
  int16_t ECEFVelocityY;
  int32_t ECEFPositionZ;
  int16_t ECEFVelocityZ;
  uint8_t XposStandardDeviation;
  uint8_t YposStandardDeviation;
  uint8_t ZposStandardDeviation;
  uint8_t XvelStandardDeviation;
  uint8_t YvelStandardDeviation;
  uint8_t ZvelStandardDeviation;
};
static_assert(sizeof(RawGPSMeasurementsTelemetryFormat) == 36);
template <> struct MessageSize<RawGPSMeasurementsTelemetryFormat> { static constexpr size_t value = 36; };
template <> struct GetID<RawGPSMeasurementsTelemetryFormat> { static constexpr int value = 210; };

struct RawStarTrackerTelemetryFormat {
  uint8_t Numberofstarsdetected;
  uint8_t Starimagenoise;
  uint8_t InvalidStars;
  uint8_t Numberofstarsidentified;
  StarIDModeValEnumerationValues Identificationmode : 8;
  uint8_t Imagedarkvalue;
  bool ImageCaptureSuccess : 1;
  bool DetectionSuccess : 1;
  bool IdentificationSuccess : 1;
  bool AttitudeSuccess : 1;
  bool ProcessingTimeError : 1;
  bool TrackingModuleEnabled : 1;
  bool PredictionEnabled : 1;
  bool Commserror : 1;
  uint16_t SamplePeriod;
  uint8_t Star1confidence;
  uint8_t Star2confidence;
  uint8_t Star3confidence;
  uint16_t MagnitudeStar1;
  uint16_t MagnitudeStar2;
  uint16_t MagnitudeStar3;
  uint16_t CatalogueStar1;
  int16_t CentroidXStar1;
  int16_t CentroidYStar1;
  uint16_t CatalogueStar2;
  int16_t CentroidXStar2;
  int16_t CentroidYStar2;
  uint16_t CatalogueStar3;
  int16_t CentroidXStar3;
  int16_t CentroidYStar3;
  uint16_t Capture;
  uint16_t Detection;
  uint16_t Identification;
  int16_t xAxisrate;
  int16_t yAxisrate;
  int16_t zAxisrate;
  int16_t Q0;
  int16_t Q1;
  int16_t Q2;
};
static_assert(sizeof(RawStarTrackerTelemetryFormat) == 54);
template <> struct MessageSize<RawStarTrackerTelemetryFormat> { static constexpr size_t value = 54; };
template <> struct GetID<RawStarTrackerTelemetryFormat> { static constexpr int value = 211; };

struct Star1RawDataTelemetryFormat {
  uint16_t CatalogueStar1;
  int16_t CentroidXStar1;
  int16_t CentroidYStar1;
};
static_assert(sizeof(Star1RawDataTelemetryFormat) == 6);
template <> struct MessageSize<Star1RawDataTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<Star1RawDataTelemetryFormat> { static constexpr int value = 212; };

struct Star2RawDataTelemetryFormat {
  uint16_t CatalogueStar2;
  int16_t CentroidXStar2;
  int16_t CentroidYStar2;
};
static_assert(sizeof(Star2RawDataTelemetryFormat) == 6);
template <> struct MessageSize<Star2RawDataTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<Star2RawDataTelemetryFormat> { static constexpr int value = 213; };

struct Star3RawDataTelemetryFormat {
  uint16_t CatalogueStar3;
  int16_t CentroidXStar3;
  int16_t CentroidYStar3;
};
static_assert(sizeof(Star3RawDataTelemetryFormat) == 6);
template <> struct MessageSize<Star3RawDataTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<Star3RawDataTelemetryFormat> { static constexpr int value = 214; };

struct SecondaryMagnetometerRawMeasurementsTelemetryFormat {
  int16_t MagX;
  int16_t MagY;
  int16_t MagZ;
};
static_assert(sizeof(SecondaryMagnetometerRawMeasurementsTelemetryFormat) == 6);
template <> struct MessageSize<SecondaryMagnetometerRawMeasurementsTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<SecondaryMagnetometerRawMeasurementsTelemetryFormat> { static constexpr int value = 215; };

struct RawRateSensorTelemetryFormat {
  int16_t RateX;
  int16_t RateY;
  int16_t RateZ;
};
static_assert(sizeof(RawRateSensorTelemetryFormat) == 6);
template <> struct MessageSize<RawRateSensorTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<RawRateSensorTelemetryFormat> { static constexpr int value = 216; };

struct EstimatedQuaternionTelemetryFormat {
  int16_t Estimatedq1;
  int16_t Estimatedq2;
  int16_t Estimatedq3;
};
static_assert(sizeof(EstimatedQuaternionTelemetryFormat) == 6);
template <> struct MessageSize<EstimatedQuaternionTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<EstimatedQuaternionTelemetryFormat> { static constexpr int value = 218; };

struct ECEFPositionTelemetryFormat {
  int16_t ECEFPositionX;
  int16_t ECEFPositionY;
  int16_t ECEFPositionZ;
};
static_assert(sizeof(ECEFPositionTelemetryFormat) == 6);
template <> struct MessageSize<ECEFPositionTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<ECEFPositionTelemetryFormat> { static constexpr int value = 219; };

struct ACPExecutionStateTelemetryFormat {
  uint16_t TimeSinceIterationStart;
  ExecutionWaypointsEnumerationValues CurrentExecutionPoint : 8;
};
static_assert(sizeof(ACPExecutionStateTelemetryFormat) == 3);
template <> struct MessageSize<ACPExecutionStateTelemetryFormat> { static constexpr size_t value = 3; };
template <> struct GetID<ACPExecutionStateTelemetryFormat> { static constexpr int value = 220; };

struct CurrentADCSState2TelemetryFormat {
  bool OrbitParametersareInvalid : 1;
  bool ConfigurationisInvalid : 1;
  bool ControlModeChangeisnotallowed : 1;
  bool EstimatorChangeisnotallowed : 1;
  MagModeValEnumerationValues CurrentMagnetometerSamplingMode : 2;
  bool Modelledandmeasuredmagneticfielddiffersinsize : 1;
  bool NodeRecoveryError : 1;
  bool CubeSense1RuntimeError : 1;
  bool CubeSense2RuntimeError : 1;
  bool CubeControlSignalRuntimeError : 1;
  bool CubeControlMotorRuntimeError : 1;
  bool CubeWheel1RuntimeError : 1;
  bool CubeWheel2RuntimeError : 1;
  bool CubeWheel3RuntimeError : 1;
  bool CubeStarRuntimeError : 1;
  bool MagnetometerError : 1;
  bool RateSensorFailure : 1;
  char unused_0[3];
};
static_assert(sizeof(CurrentADCSState2TelemetryFormat) == 6);
template <> struct MessageSize<CurrentADCSState2TelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<CurrentADCSState2TelemetryFormat> { static constexpr int value = 224; };

struct ASGP4TLEsTelemetryFormat {
  bool ASGP4complete : 1;
  AsgpErrorEnumerationValues ASGP4error : 7;
  float ASGP4Epoch;
  float ASGP4inclination;
  float ASGP4RAAN;
  float ASGP4ECC;
  float ASGP4AOP;
  float ASGP4MA;
  float ASGP4MM;
  float ASGP4Bstar;
};
static_assert(sizeof(ASGP4TLEsTelemetryFormat) == 33);
template <> struct MessageSize<ASGP4TLEsTelemetryFormat> { static constexpr size_t value = 33; };
template <> struct GetID<ASGP4TLEsTelemetryFormat> { static constexpr int value = 228; };

struct CubeStarEstimatedRatesTelemetryFormat {
  int16_t xAxisrate;
  int16_t yAxisrate;
  int16_t zAxisrate;
};
static_assert(sizeof(CubeStarEstimatedRatesTelemetryFormat) == 6);
template <> struct MessageSize<CubeStarEstimatedRatesTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<CubeStarEstimatedRatesTelemetryFormat> { static constexpr int value = 229; };

struct CubeStarEstimatedQuaternionTelemetryFormat {
  int16_t Q0;
  int16_t Q1;
  int16_t Q2;
};
static_assert(sizeof(CubeStarEstimatedQuaternionTelemetryFormat) == 6);
template <> struct MessageSize<CubeStarEstimatedQuaternionTelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<CubeStarEstimatedQuaternionTelemetryFormat> { static constexpr int value = 230; };

struct StarPerformance2TelemetryFormat {
  bool ImageCaptureSuccess : 1;
  bool DetectionSuccess : 1;
  bool IdentificationSuccess : 1;
  bool AttitudeSuccess : 1;
  bool ProcessingTimeError : 1;
  bool TrackingModuleEnabled : 1;
  bool PredictionEnabled : 1;
  bool Commserror : 1;
  uint16_t SamplePeriod;
  uint8_t Star1confidence;
  uint8_t Star2confidence;
  uint8_t Star3confidence;
};
static_assert(sizeof(StarPerformance2TelemetryFormat) == 6);
template <> struct MessageSize<StarPerformance2TelemetryFormat> { static constexpr size_t value = 6; };
template <> struct GetID<StarPerformance2TelemetryFormat> { static constexpr int value = 231; };

struct CubeSense2CurrentMeasurementsTelemetryFormat {
  uint16_t CubeSense23V3Current;
  uint16_t CubeSense2CamSRAMCurrent;
};
static_assert(sizeof(CubeSense2CurrentMeasurementsTelemetryFormat) == 4);
template <> struct MessageSize<CubeSense2CurrentMeasurementsTelemetryFormat> { static constexpr size_t value = 4; };
template <> struct GetID<CubeSense2CurrentMeasurementsTelemetryFormat> { static constexpr int value = 232; };

struct StatusofImageCaptureandSaveOperationTelemetryFormat {
  uint8_t PercentageComplete;
  ImSaveStatusEnumerationValues Status : 8;
};
static_assert(sizeof(StatusofImageCaptureandSaveOperationTelemetryFormat) == 2);
template <> struct MessageSize<StatusofImageCaptureandSaveOperationTelemetryFormat> { static constexpr size_t value = 2; };
template <> struct GetID<StatusofImageCaptureandSaveOperationTelemetryFormat> { static constexpr int value = 233; };

struct SetMagnetorquerConfigurationMessageFormat {
  AxisSelectEnumerationValues Magnetorquer1Configuration : 8;
  AxisSelectEnumerationValues Magnetorquer2Configuration : 8;
  AxisSelectEnumerationValues Magnetorquer3Configuration : 8;
};
static_assert(sizeof(SetMagnetorquerConfigurationMessageFormat) == 3);
template <> struct MessageSize<SetMagnetorquerConfigurationMessageFormat> { static constexpr size_t value = 3; };
template <> struct SetID<SetMagnetorquerConfigurationMessageFormat> { static constexpr int value = 21; };
template <> struct GetID<SetMagnetorquerConfigurationMessageFormat> { static constexpr int value = 136; };

struct SetWheelConfigurationMessageFormat {
  AxisSelectEnumerationValues RW1Configuration : 8;
  AxisSelectEnumerationValues RW2Configuration : 8;
  AxisSelectEnumerationValues RW3Configuration : 8;
  AxisSelectEnumerationValues RW4Configuration : 8;
};
static_assert(sizeof(SetWheelConfigurationMessageFormat) == 4);
template <> struct MessageSize<SetWheelConfigurationMessageFormat> { static constexpr size_t value = 4; };
template <> struct SetID<SetWheelConfigurationMessageFormat> { static constexpr int value = 22; };
template <> struct GetID<SetWheelConfigurationMessageFormat> { static constexpr int value = 137; };

struct SetRateGyroConfigurationMessageFormat {
  AxisSelectEnumerationValues Gyro1Configuration : 8;
  AxisSelectEnumerationValues Gyro2Configuration : 8;
  AxisSelectEnumerationValues Gyro3Configuration : 8;
  int16_t XRateSensorOffset;
  int16_t YRateSensorOffset;
  int16_t ZRateSensorOffset;
  uint8_t RateSensorMult;
};
static_assert(sizeof(SetRateGyroConfigurationMessageFormat) == 10);
template <> struct MessageSize<SetRateGyroConfigurationMessageFormat> { static constexpr size_t value = 10; };
template <> struct SetID<SetRateGyroConfigurationMessageFormat> { static constexpr int value = 23; };
template <> struct GetID<SetRateGyroConfigurationMessageFormat> { static constexpr int value = 138; };

struct CSSConfigurationMessageFormat {
  AxisSelectEnumerationValues CSS1Configuration : 8;
  AxisSelectEnumerationValues CSS2Configuration : 8;
  AxisSelectEnumerationValues CSS3Configuration : 8;
  AxisSelectEnumerationValues CSS4Configuration : 8;
  AxisSelectEnumerationValues CSS5Configuration : 8;
  AxisSelectEnumerationValues CSS6Configuration : 8;
  AxisSelectEnumerationValues CSS7Configuration : 8;
  AxisSelectEnumerationValues CSS8Configuration : 8;
  AxisSelectEnumerationValues CSS9Configuration : 8;
  AxisSelectEnumerationValues CSS10Configuration : 8;
  uint8_t CSS1RelativeScale;
  uint8_t CSS2RelativeScale;
  uint8_t CSS3RelativeScale;
  uint8_t CSS4RelativeScale;
  uint8_t CSS5RelativeScale;
  uint8_t CSS6RelativeScale;
  uint8_t CSS7RelativeScale;
  uint8_t CSS8RelativeScale;
  uint8_t CSS9RelativeScale;
  uint8_t CSS10RelativeScale;
  uint8_t CSSThreshold;
};
static_assert(sizeof(CSSConfigurationMessageFormat) == 21);
template <> struct MessageSize<CSSConfigurationMessageFormat> { static constexpr size_t value = 21; };
template <> struct SetID<CSSConfigurationMessageFormat> { static constexpr int value = 24; };
template <> struct GetID<CSSConfigurationMessageFormat> { static constexpr int value = 139; };

struct ADCSPowerControlMessageFormat {
  PowerSelectEnumerationValues CubeControlSignalPowerSelection : 2;
  PowerSelectEnumerationValues CubeControlMotorPowerSelection : 2;
  PowerSelectEnumerationValues CubeSense1PowerSelection : 2;
  PowerSelectEnumerationValues CubeSense2PowerSelection : 2;
  PowerSelectEnumerationValues CubeStarPowerPowerSelection : 2;
  PowerSelectEnumerationValues CubeWheel1PowerPowerSelection : 2;
  PowerSelectEnumerationValues CubeWheel2PowerPowerSelection : 2;
  PowerSelectEnumerationValues CubeWheel3PowerPowerSelection : 2;
  PowerSelectEnumerationValues MotorPower : 2;
  PowerSelectEnumerationValues GPSPower : 2;
};
static_assert(sizeof(ADCSPowerControlMessageFormat) == 3);
template <> struct MessageSize<ADCSPowerControlMessageFormat> { static constexpr size_t value = 3; };
template <> struct SetID<ADCSPowerControlMessageFormat> { static constexpr int value = 11; };
template <> struct GetID<ADCSPowerControlMessageFormat> { static constexpr int value = 197; };

struct CommandedAttitudeAnglesMessageFormat {
  int16_t CommandedRollAngle;
  int16_t CommandedPitchAngle;
  int16_t CommandedYawAngle;
};
static_assert(sizeof(CommandedAttitudeAnglesMessageFormat) == 6);
template <> struct MessageSize<CommandedAttitudeAnglesMessageFormat> { static constexpr size_t value = 6; };
template <> struct SetID<CommandedAttitudeAnglesMessageFormat> { static constexpr int value = 15; };
template <> struct GetID<CommandedAttitudeAnglesMessageFormat> { static constexpr int value = 199; };

struct TrackingControllerTargetReferenceMessageFormat {
  float Geocentriclongitudeoftarget;
  float Geocentriclatitudeoftarget;
  float Geocentricaltitudeoftarget;
};
static_assert(sizeof(TrackingControllerTargetReferenceMessageFormat) == 12);
template <> struct MessageSize<TrackingControllerTargetReferenceMessageFormat> { static constexpr size_t value = 12; };
template <> struct SetID<TrackingControllerTargetReferenceMessageFormat> { static constexpr int value = 55; };
template <> struct GetID<TrackingControllerTargetReferenceMessageFormat> { static constexpr int value = 200; };

struct SetStarTrackerConfigurationMessageFormat {
  int16_t StarTrackerMountingTransformAlphaAngle;
  int16_t StarTrackerMountingTransformBetaAngle;
  int16_t StarTrackerMountingTransformGammaAngle;
  uint16_t StarTrackerexposuretime;
  uint16_t StarTrackeranaloggain;
  uint8_t StarTrackerdetectionthreshold;
  uint8_t StarTrackerstarthreshold;
  uint8_t MaximumStarMatched;
  uint16_t DetectionTimeoutduration;
  uint8_t MaximumStarPixel;
  uint8_t MinimumStarPixel;
  uint8_t StarTrackerErrorMargin;
  uint16_t StarTrackerDelayTime;
  float StarTrackerCentroidX;
  float StarTrackerCentroidY;
  float StarTrackerFocalLength;
  float K1radialdistortioncoefficient;
  float K2radialdistortioncoefficient;
  float P1tangentialdistortioncoefficient;
  float P2tangentialdistortioncoefficient;
  uint8_t Startrackingwindowwidth;
  uint8_t StarTrackingMargin;
  uint8_t StarValidationMargin;
  bool StarTrackingModuleEnable : 1;
  bool StarTrackingLocationPredictionEnable : 1;
  char : 0;
  uint8_t StarTrackingSearchWidth;
};
static_assert(sizeof(SetStarTrackerConfigurationMessageFormat) == 53);
template <> struct MessageSize<SetStarTrackerConfigurationMessageFormat> { static constexpr size_t value = 53; };
template <> struct SetID<SetStarTrackerConfigurationMessageFormat> { static constexpr int value = 37; };
template <> struct GetID<SetStarTrackerConfigurationMessageFormat> { static constexpr int value = 202; };

struct CubeSenseConfigurationMessageFormat {
  int16_t Cam1SensorMountingTransformAlphaAngle;
  int16_t Cam1SensorMountingTransformBetaAngle;
  int16_t Cam1SensorMountingTransformGammaAngle;
  uint8_t Cam1detectionthreshold;
  bool Cam1sensorautoadjustmode : 1;
  char : 0;
  uint16_t Cam1sensorexposuretime;
  uint16_t Cam1BoresightX;
  uint16_t Cam1BoresightY;
  int16_t Cam2SensorMountingTransformAlphaAngle;
  int16_t Cam2SensorMountingTransformBetaAngle;
  int16_t Cam2SensorMountingTransformGammaAngle;
  uint8_t Cam2detectionthreshold;
  bool Cam2sensorautoadjustmode : 1;
  char : 0;
  uint16_t Cam2sensorexposuretime;
  uint16_t Cam2BoresightX;
  uint16_t Cam2BoresightY;
  uint8_t NadirMaxDeviationPercentage;
  uint8_t NadirMaxBadEdges;
  uint8_t NadirMaxRadius;
  uint8_t NadirMinRadius;
  uint16_t Cam1MinimumXofarea1;
  uint16_t Cam1MaximumXofarea1;
  uint16_t Cam1MinimumYofarea1;
  uint16_t Cam1MaximumYofarea1;
  uint16_t Cam1MinimumXofarea2;
  uint16_t Cam1MaximumXofarea2;
  uint16_t Cam1MinimumYofarea2;
  uint16_t Cam1MaximumYofarea2;
  uint16_t Cam1MinimumXofarea3;
  uint16_t Cam1MaximumXofarea3;
  uint16_t Cam1MinimumYofarea3;
  uint16_t Cam1MaximumYofarea3;
  uint16_t Cam1MinimumXofarea4;
  uint16_t Cam1MaximumXofarea4;
  uint16_t Cam1MinimumYofarea4;
  uint16_t Cam1MaximumYofarea4;
  uint16_t Cam1MinimumXofarea5;
  uint16_t Cam1MaximumXofarea5;
  uint16_t Cam1MinimumYofarea5;
  uint16_t Cam1MaximumYofarea5;
  uint16_t Cam2MinimumXofarea1;
  uint16_t Cam2MaximumXofarea1;
  uint16_t Cam2MinimumYofarea1;
  uint16_t Cam2MaximumYofarea1;
  uint16_t Cam2MinimumXofarea2;
  uint16_t Cam2MaximumXofarea2;
  uint16_t Cam2MinimumYofarea2;
  uint16_t Cam2MaximumYofarea2;
  uint16_t Cam2MinimumXofarea3;
  uint16_t Cam2MaximumXofarea3;
  uint16_t Cam2MinimumYofarea3;
  uint16_t Cam2MaximumYofarea3;
  uint16_t Cam2MinimumXofarea4;
  uint16_t Cam2MaximumXofarea4;
  uint16_t Cam2MinimumYofarea4;
  uint16_t Cam2MaximumYofarea4;
  uint16_t Cam2MinimumXofarea5;
  uint16_t Cam2MaximumXofarea5;
  uint16_t Cam2MinimumYofarea5;
  uint16_t Cam2MaximumYofarea5;
};
static_assert(sizeof(CubeSenseConfigurationMessageFormat) == 112);
template <> struct MessageSize<CubeSenseConfigurationMessageFormat> { static constexpr size_t value = 112; };
template <> struct SetID<CubeSenseConfigurationMessageFormat> { static constexpr int value = 25; };
template <> struct GetID<CubeSenseConfigurationMessageFormat> { static constexpr int value = 203; };

struct MagnetometerConfigurationMessageFormat {
  int16_t MagnetometerMountingTransformAlphaAngle;
  int16_t MagnetometerMountingTransformBetaAngle;
  int16_t MagnetometerMountingTransformGammaAngle;
  int16_t MagnetometerChannel1Offset;
  int16_t MagnetometerChannel2Offset;
  int16_t MagnetometerChannel3Offset;
  int16_t MagnetometerSensitivityMatrixS11;
  int16_t MagnetometerSensitivityMatrixS22;
  int16_t MagnetometerSensitivityMatrixS33;
  int16_t MagnetometerSensitivityMatrixS12;
  int16_t MagnetometerSensitivityMatrixS13;
  int16_t MagnetometerSensitivityMatrixS21;
  int16_t MagnetometerSensitivityMatrixS23;
  int16_t MagnetometerSensitivityMatrixS31;
  int16_t MagnetometerSensitivityMatrixS32;
};
static_assert(sizeof(MagnetometerConfigurationMessageFormat) == 30);
template <> struct MessageSize<MagnetometerConfigurationMessageFormat> { static constexpr size_t value = 30; };
template <> struct SetID<MagnetometerConfigurationMessageFormat> { static constexpr int value = 26; };
template <> struct GetID<MagnetometerConfigurationMessageFormat> { static constexpr int value = 204; };

struct RedundantMagnetometerConfigurationMessageFormat {
  int16_t RedundantMagnetometerMountingTransformAlphaAngle;
  int16_t RedundantMagnetometerMountingTransformBetaAngle;
  int16_t RedundantMagnetometerMountingTransformGammaAngle;
  int16_t RedundantMagnetometerChannel1Offset;
  int16_t RedundantMagnetometerChannel2Offset;
  int16_t RedundantMagnetometerChannel3Offset;
  int16_t RedundantMagnetometerSensitivityMatrixS11;
  int16_t RedundantMagnetometerSensitivityMatrixS22;
  int16_t RedundantMagnetometerSensitivityMatrixS33;
  int16_t RedundantMagnetometerSensitivityMatrixS12;
  int16_t RedundantMagnetometerSensitivityMatrixS13;
  int16_t RedundantMagnetometerSensitivityMatrixS21;
  int16_t RedundantMagnetometerSensitivityMatrixS23;
  int16_t RedundantMagnetometerSensitivityMatrixS31;
  int16_t RedundantMagnetometerSensitivityMatrixS32;
};
static_assert(sizeof(RedundantMagnetometerConfigurationMessageFormat) == 30);
template <> struct MessageSize<RedundantMagnetometerConfigurationMessageFormat> { static constexpr size_t value = 30; };
template <> struct SetID<RedundantMagnetometerConfigurationMessageFormat> { static constexpr int value = 36; };
template <> struct GetID<RedundantMagnetometerConfigurationMessageFormat> { static constexpr int value = 205; };

struct ADCSConfigurationMessageFormat {
  AxisSelectEnumerationValues Magnetorquer1Configuration : 8;
  AxisSelectEnumerationValues Magnetorquer2Configuration : 8;
  AxisSelectEnumerationValues Magnetorquer3Configuration : 8;
  AxisSelectEnumerationValues RW1Configuration : 8;
  AxisSelectEnumerationValues RW2Configuration : 8;
  AxisSelectEnumerationValues RW3Configuration : 8;
  AxisSelectEnumerationValues RW4Configuration : 8;
  AxisSelectEnumerationValues Gyro1Configuration : 8;
  AxisSelectEnumerationValues Gyro2Configuration : 8;
  AxisSelectEnumerationValues Gyro3Configuration : 8;
  int16_t XRateSensorOffset;
  int16_t YRateSensorOffset;
  int16_t ZRateSensorOffset;
  uint8_t RateSensorMult;
  AxisSelectEnumerationValues CSS1Configuration : 8;
  AxisSelectEnumerationValues CSS2Configuration : 8;
  AxisSelectEnumerationValues CSS3Configuration : 8;
  AxisSelectEnumerationValues CSS4Configuration : 8;
  AxisSelectEnumerationValues CSS5Configuration : 8;
  AxisSelectEnumerationValues CSS6Configuration : 8;
  AxisSelectEnumerationValues CSS7Configuration : 8;
  AxisSelectEnumerationValues CSS8Configuration : 8;
  AxisSelectEnumerationValues CSS9Configuration : 8;
  AxisSelectEnumerationValues CSS10Configuration : 8;
  uint8_t CSS1RelativeScale;
  uint8_t CSS2RelativeScale;
  uint8_t CSS3RelativeScale;
  uint8_t CSS4RelativeScale;
  uint8_t CSS5RelativeScale;
  uint8_t CSS6RelativeScale;
  uint8_t CSS7RelativeScale;
  uint8_t CSS8RelativeScale;
  uint8_t CSS9RelativeScale;
  uint8_t CSS10RelativeScale;
  uint8_t CSSThreshold;
  int16_t Cam1SensorMountingTransformAlphaAngle;
  int16_t Cam1SensorMountingTransformBetaAngle;
  int16_t Cam1SensorMountingTransformGammaAngle;
  uint8_t Cam1detectionthreshold;
  bool Cam1sensorautoadjustmode : 1;
  char : 0;
  uint16_t Cam1sensorexposuretime;
  uint16_t Cam1BoresightX;
  uint16_t Cam1BoresightY;
  int16_t Cam2SensorMountingTransformAlphaAngle;
  int16_t Cam2SensorMountingTransformBetaAngle;
  int16_t Cam2SensorMountingTransformGammaAngle;
  uint8_t Cam2detectionthreshold;
  bool Cam2sensorautoadjustmode : 1;
  char : 0;
  uint16_t Cam2sensorexposuretime;
  uint16_t Cam2BoresightX;
  uint16_t Cam2BoresightY;
  uint8_t NadirMaxDeviationPercentage;
  uint8_t NadirMaxBadEdges;
  uint8_t NadirMaxRadius;
  uint8_t NadirMinRadius;
  uint16_t Cam1MinimumXofarea1;
  uint16_t Cam1MaximumXofarea1;
  uint16_t Cam1MinimumYofarea1;
  uint16_t Cam1MaximumYofarea1;
  uint16_t Cam1MinimumXofarea2;
  uint16_t Cam1MaximumXofarea2;
  uint16_t Cam1MinimumYofarea2;
  uint16_t Cam1MaximumYofarea2;
  uint16_t Cam1MinimumXofarea3;
  uint16_t Cam1MaximumXofarea3;
  uint16_t Cam1MinimumYofarea3;
  uint16_t Cam1MaximumYofarea3;
  uint16_t Cam1MinimumXofarea4;
  uint16_t Cam1MaximumXofarea4;
  uint16_t Cam1MinimumYofarea4;
  uint16_t Cam1MaximumYofarea4;
  uint16_t Cam1MinimumXofarea5;
  uint16_t Cam1MaximumXofarea5;
  uint16_t Cam1MinimumYofarea5;
  uint16_t Cam1MaximumYofarea5;
  uint16_t Cam2MinimumXofarea1;
  uint16_t Cam2MaximumXofarea1;
  uint16_t Cam2MinimumYofarea1;
  uint16_t Cam2MaximumYofarea1;
  uint16_t Cam2MinimumXofarea2;
  uint16_t Cam2MaximumXofarea2;
  uint16_t Cam2MinimumYofarea2;
  uint16_t Cam2MaximumYofarea2;
  uint16_t Cam2MinimumXofarea3;
  uint16_t Cam2MaximumXofarea3;
  uint16_t Cam2MinimumYofarea3;
  uint16_t Cam2MaximumYofarea3;
  uint16_t Cam2MinimumXofarea4;
  uint16_t Cam2MaximumXofarea4;
  uint16_t Cam2MinimumYofarea4;
  uint16_t Cam2MaximumYofarea4;
  uint16_t Cam2MinimumXofarea5;
  uint16_t Cam2MaximumXofarea5;
  uint16_t Cam2MinimumYofarea5;
  uint16_t Cam2MaximumYofarea5;
  int16_t MagnetometerMountingTransformAlphaAngle;
  int16_t MagnetometerMountingTransformBetaAngle;
  int16_t MagnetometerMountingTransformGammaAngle;
  int16_t MagnetometerChannel1Offset;
  int16_t MagnetometerChannel2Offset;
  int16_t MagnetometerChannel3Offset;
  int16_t MagnetometerSensitivityMatrixS11;
  int16_t MagnetometerSensitivityMatrixS22;
  int16_t MagnetometerSensitivityMatrixS33;
  int16_t MagnetometerSensitivityMatrixS12;
  int16_t MagnetometerSensitivityMatrixS13;
  int16_t MagnetometerSensitivityMatrixS21;
  int16_t MagnetometerSensitivityMatrixS23;
  int16_t MagnetometerSensitivityMatrixS31;
  int16_t MagnetometerSensitivityMatrixS32;
  int16_t RedundantMagnetometerMountingTransformAlphaAngle;
  int16_t RedundantMagnetometerMountingTransformBetaAngle;
  int16_t RedundantMagnetometerMountingTransformGammaAngle;
  int16_t RedundantMagnetometerChannel1Offset;
  int16_t RedundantMagnetometerChannel2Offset;
  int16_t RedundantMagnetometerChannel3Offset;
  int16_t RedundantMagnetometerSensitivityMatrixS11;
  int16_t RedundantMagnetometerSensitivityMatrixS22;
  int16_t RedundantMagnetometerSensitivityMatrixS33;
  int16_t RedundantMagnetometerSensitivityMatrixS12;
  int16_t RedundantMagnetometerSensitivityMatrixS13;
  int16_t RedundantMagnetometerSensitivityMatrixS21;
  int16_t RedundantMagnetometerSensitivityMatrixS23;
  int16_t RedundantMagnetometerSensitivityMatrixS31;
  int16_t RedundantMagnetometerSensitivityMatrixS32;
  int16_t StarTrackerMountingTransformAlphaAngle;
  int16_t StarTrackerMountingTransformBetaAngle;
  int16_t StarTrackerMountingTransformGammaAngle;
  uint16_t StarTrackerexposuretime;
  uint16_t StarTrackeranaloggain;
  uint8_t StarTrackerdetectionthreshold;
  uint8_t StarTrackerstarthreshold;
  uint8_t MaximumStarMatched;
  uint16_t DetectionTimeoutduration;
  uint8_t MaximumStarPixel;
  uint8_t MinimumStarPixel;
  uint8_t StarTrackerErrorMargin;
  uint16_t StarTrackerDelayTime;
  float StarTrackerCentroidX;
  float StarTrackerCentroidY;
  float StarTrackerFocalLength;
  float K1radialdistortioncoefficient;
  float K2radialdistortioncoefficient;
  float P1tangentialdistortioncoefficient;
  float P2tangentialdistortioncoefficient;
  uint8_t Startrackingwindowwidth;
  uint8_t StarTrackingMargin;
  uint8_t StarValidationMargin;
  bool StarTrackingModuleEnable : 1;
  bool StarTrackingLocationPredictionEnable : 1;
  char : 0;
  uint8_t StarTrackingSearchWidth;
  float DetumblingSpinGain;
  float DetumblingDampingGain;
  int16_t Referencespinrate;
  float FastBDotDetumblingGain;
  float YMomentumControlGain;
  float YmomentumNutationDampingGain;
  float YmomentumProportionalGain;
  float YmomentumDerivativeGain;
  float ReferenceWheelMomentum;
  float RWheelProportionalGain;
  float RWheelDerivativeGain;
  float YWheelBiasMomentum;
  AxisSelectEnumerationValues SunpointingFacet : 7;
  bool AutomaticControlTransitionduetoWheelErrors : 1;
  float TrackingProportionalGain;
  float TrackingDerivativeGain;
  float TrackingIntegralGain;
  AxisSelectEnumerationValues TargettrackingFacet : 8;
  float MomentOfInertiaIxx;
  float MomentOfInertiaIyy;
  float MomentOfInertiaIzz;
  float ProductOfInertiaIxy;
  float ProductOfInertiaIxz;
  float ProductOfInertiaIyz;
  float MagnetometerRateFilterSystemNoise;
  float EKFSystemNoise;
  float CSSMeasurementNoise;
  float SunSensorMeasurementNoise;
  float NadirSensorMeasurementNoise;
  float MagnetometerMeasurementNoise;
  float StarTrackerMeasurementNoise;
  bool UseSunSensor : 1;
  bool UseNadirSensor : 1;
  bool UseCSS : 1;
  bool UseStarTracker : 1;
  bool Nadirsensorterminatortest : 1;
  bool AutomaticMagnetometerRecovery : 1;
  MagModeValEnumerationValues MagnetometerMode : 2;
  MagModeValEnumerationValues MagnetometerSelectionforRAWMTMTLM : 2;
  bool AutomaticEstimationTransitionduetoRateSensorErrors : 1;
  char : 0;
  uint8_t Cam1andCam2SamplingPeriod;
  uint16_t Inclcoefficient;
  uint16_t Raancoefficient;
  uint16_t Ecccoefficient;
  uint16_t Aopcoefficient;
  uint16_t Timecoefficient;
  uint16_t Poscoefficient;
  uint8_t Maximumpositionerror;
  AsgpFilterEnumerationValues ASGP4filter : 8;
  int32_t xpcoefficient;
  int32_t ypcoefficient;
  uint8_t GPSrollover;
  uint8_t Positionsd;
  uint8_t Velocitysd;
  uint8_t Minsatellites;
  uint8_t Timegain;
  uint8_t Maxlag;
  uint16_t Minsamples;
  char UserCodedControllerSettings[48];
  char UserCodedEstimatorSettings[48];
};
static_assert(sizeof(ADCSConfigurationMessageFormat) == 504);
template <> struct MessageSize<ADCSConfigurationMessageFormat> { static constexpr size_t value = 504; };
template <> struct SetID<ADCSConfigurationMessageFormat> { static constexpr int value = 20; };
template <> struct GetID<ADCSConfigurationMessageFormat> { static constexpr int value = 206; };

struct SGP4OrbitParametersMessageFormat {
  double Inclination;
  double Eccentricity;
  double RightascensionoftheAscendingNode;
  double ArgumentofPerigee;
  double BStardragterm;
  double MeanMotion;
  double MeanAnomaly;
  double Epoch;
};
static_assert(sizeof(SGP4OrbitParametersMessageFormat) == 64);
template <> struct MessageSize<SGP4OrbitParametersMessageFormat> { static constexpr size_t value = 64; };
template <> struct SetID<SGP4OrbitParametersMessageFormat> { static constexpr int value = 45; };
template <> struct GetID<SGP4OrbitParametersMessageFormat> { static constexpr int value = 207; };

struct SetDetumblingControlParametersMessageFormat {
  float DetumblingSpinGain;
  float DetumblingDampingGain;
  int16_t Referencespinrate;
  float FastBDotDetumblingGain;
};
static_assert(sizeof(SetDetumblingControlParametersMessageFormat) == 14);
template <> struct MessageSize<SetDetumblingControlParametersMessageFormat> { static constexpr size_t value = 14; };
template <> struct SetID<SetDetumblingControlParametersMessageFormat> { static constexpr int value = 38; };
template <> struct GetID<SetDetumblingControlParametersMessageFormat> { static constexpr int value = 208; };

struct SetYWheelControlParametersMessageFormat {
  float YMomentumControlGain;
  float YmomentumNutationDampingGain;
  float YmomentumProportionalGain;
  float YmomentumDerivativeGain;
  float ReferenceWheelMomentum;
};
static_assert(sizeof(SetYWheelControlParametersMessageFormat) == 20);
template <> struct MessageSize<SetYWheelControlParametersMessageFormat> { static constexpr size_t value = 20; };
template <> struct SetID<SetYWheelControlParametersMessageFormat> { static constexpr int value = 39; };
template <> struct GetID<SetYWheelControlParametersMessageFormat> { static constexpr int value = 209; };

struct SetReactionWheelControlParametersMessageFormat {
  float RWheelProportionalGain;
  float RWheelDerivativeGain;
  float YWheelBiasMomentum;
  AxisSelectEnumerationValues SunpointingFacet : 7;
  bool AutomaticControlTransitionduetoWheelErrors : 1;
};
static_assert(sizeof(SetReactionWheelControlParametersMessageFormat) == 13);
template <> struct MessageSize<SetReactionWheelControlParametersMessageFormat> { static constexpr size_t value = 13; };
template <> struct SetID<SetReactionWheelControlParametersMessageFormat> { static constexpr int value = 40; };
template <> struct GetID<SetReactionWheelControlParametersMessageFormat> { static constexpr int value = 217; };

struct SetTrackingControllerGainParametersMessageFormat {
  float TrackingProportionalGain;
  float TrackingDerivativeGain;
  float TrackingIntegralGain;
  AxisSelectEnumerationValues TargettrackingFacet : 8;
};
static_assert(sizeof(SetTrackingControllerGainParametersMessageFormat) == 13);
template <> struct MessageSize<SetTrackingControllerGainParametersMessageFormat> { static constexpr size_t value = 13; };
template <> struct SetID<SetTrackingControllerGainParametersMessageFormat> { static constexpr int value = 54; };
template <> struct GetID<SetTrackingControllerGainParametersMessageFormat> { static constexpr int value = 221; };

struct MomentofInertiaMatrixMessageFormat {
  float MomentOfInertiaIxx;
  float MomentOfInertiaIyy;
  float MomentOfInertiaIzz;
  float ProductOfInertiaIxy;
  float ProductOfInertiaIxz;
  float ProductOfInertiaIyz;
};
static_assert(sizeof(MomentofInertiaMatrixMessageFormat) == 24);
template <> struct MessageSize<MomentofInertiaMatrixMessageFormat> { static constexpr size_t value = 24; };
template <> struct SetID<MomentofInertiaMatrixMessageFormat> { static constexpr int value = 41; };
template <> struct GetID<MomentofInertiaMatrixMessageFormat> { static constexpr int value = 222; };

struct EstimationParametersMessageFormat {
  float MagnetometerRateFilterSystemNoise;
  float EKFSystemNoise;
  float CSSMeasurementNoise;
  float SunSensorMeasurementNoise;
  float NadirSensorMeasurementNoise;
  float MagnetometerMeasurementNoise;
  float StarTrackerMeasurementNoise;
  bool UseSunSensor : 1;
  bool UseNadirSensor : 1;
  bool UseCSS : 1;
  bool UseStarTracker : 1;
  bool Nadirsensorterminatortest : 1;
  bool AutomaticMagnetometerRecovery : 1;
  MagModeValEnumerationValues MagnetometerMode : 2;
  MagModeValEnumerationValues MagnetometerSelectionforRAWMTMTLM : 2;
  bool AutomaticEstimationTransitionduetoRateSensorErrors : 1;
  char : 0;
  uint8_t Cam1andCam2SamplingPeriod;
};
static_assert(sizeof(EstimationParametersMessageFormat) == 31);
template <> struct MessageSize<EstimationParametersMessageFormat> { static constexpr size_t value = 31; };
template <> struct SetID<EstimationParametersMessageFormat> { static constexpr int value = 27; };
template <> struct GetID<EstimationParametersMessageFormat> { static constexpr int value = 223; };

struct ADCSSystemConfigurationMessageFormat {
  AcpProgramTypeEnumerationValues ACPType : 4;
  SpecialConSelectEnumerationValues SpecialControlSelection : 4;
  uint8_t CubeControlSignalVersion;
  uint8_t CubeControlMotorVersion;
  uint8_t CubeSense1Version;
  uint8_t CubeSense2Version;
  CsCamTypeEnumerationValues CubeSense1CameraType : 4;
  CsCamTypeEnumerationValues CubeSense2CameraType : 4;
  uint8_t CubeStarVersion;
  GpsSelectEnumerationValues GPSType : 4;
  bool RedundantMTMIncluded : 1;
  char : 0;
  float MagnetorquerXMaxDipole;
  float MagnetorquerYMaxDipole;
  float MagnetorquerZMaxDipole;
  float MagnetorquerOntimeResolution;
  float MagnetorquerMaximumOntime;
  float RWXMaximumTorque;
  float RWYMaximumTorque;
  float RWZMaximumTorque;
  float RWXMaximumMomentum;
  float RWYMaximumMomentum;
  float RWZMaximumMomentum;
  float RWXInertia;
  float RWYInertia;
  float RWZInertia;
  float RWTorqueIncrement;
  float PrimaryMTMXBiasd1;
  float PrimaryMTMYBiasd1;
  float PrimaryMTMZBiasd1;
  float PrimaryMTMXBiasd2;
  float PrimaryMTMYBiasd2;
  float PrimaryMTMZBiasd2;
  float PrimaryMTMXSenss1;
  float PrimaryMTMYSenss1;
  float PrimaryMTMZSenss1;
  float PrimaryMTMXSenss2;
  float PrimaryMTMYSenss2;
  float PrimaryMTMZSenss2;
  float RedundantMTMXBiasd1;
  float RedundantMTMYBiasd1;
  float RedundantMTMZBiasd1;
  float RedundantMTMXBiasd2;
  float RedundantMTMYBiasd2;
  float RedundantMTMZBiasd2;
  float RedundantMTMXSenss1;
  float RedundantMTMYSenss1;
  float RedundantMTMZSenss1;
  float RedundantMTMXSenss2;
  float RedundantMTMYSenss2;
  float RedundantMTMZSenss2;
  GpioPortEnumerationValues CCSignalEnablePort : 4;
  GpioPortPinEnumerationValues CCSignalEnablePin : 4;
  GpioPortEnumerationValues CCMotorEnablePort : 4;
  GpioPortPinEnumerationValues CCMotorEnablePin : 4;
  GpioPortEnumerationValues CCCommonEnablePort : 4;
  GpioPortPinEnumerationValues CCCommonEnablePin : 4;
  GpioPortEnumerationValues CubeSense1EnablePort : 4;
  GpioPortPinEnumerationValues CubeSense1EnablePin : 4;
  GpioPortEnumerationValues CubeSense2EnablePort : 4;
  GpioPortPinEnumerationValues CubeSense2EnablePin : 4;
  GpioPortEnumerationValues CubeStarEnablePort : 4;
  GpioPortPinEnumerationValues CubeStarEnablePin : 4;
  GpioPortEnumerationValues CubeWheel1EnablePort : 4;
  GpioPortPinEnumerationValues CubeWheel1EnablePin : 4;
  GpioPortEnumerationValues CubeWheel2EnablePort : 4;
  GpioPortPinEnumerationValues CubeWheel2EnablePin : 4;
  GpioPortEnumerationValues CubeWheel3EnablePort : 4;
  GpioPortPinEnumerationValues CubeWheel3EnablePin : 4;
};
static_assert(sizeof(ADCSSystemConfigurationMessageFormat) == 173);
template <> struct MessageSize<ADCSSystemConfigurationMessageFormat> { static constexpr size_t value = 173; };
template <> struct SetID<ADCSSystemConfigurationMessageFormat> { static constexpr int value = 30; };
template <> struct GetID<ADCSSystemConfigurationMessageFormat> { static constexpr int value = 225; };

struct UsercodedControllerandEstimatorParametersMessageFormat {
  char UserCodedControllerSettings[48];
  char UserCodedEstimatorSettings[48];
};
static_assert(sizeof(UsercodedControllerandEstimatorParametersMessageFormat) == 96);
template <> struct MessageSize<UsercodedControllerandEstimatorParametersMessageFormat> { static constexpr size_t value = 96; };
template <> struct SetID<UsercodedControllerandEstimatorParametersMessageFormat> { static constexpr int value = 29; };
template <> struct GetID<UsercodedControllerandEstimatorParametersMessageFormat> { static constexpr int value = 226; };

struct AugmentedSGP4ParametersMessageFormat {
  uint16_t Inclcoefficient;
  uint16_t Raancoefficient;
  uint16_t Ecccoefficient;
  uint16_t Aopcoefficient;
  uint16_t Timecoefficient;
  uint16_t Poscoefficient;
  uint8_t Maximumpositionerror;
  AsgpFilterEnumerationValues ASGP4filter : 8;
  int32_t xpcoefficient;
  int32_t ypcoefficient;
  uint8_t GPSrollover;
  uint8_t Positionsd;
  uint8_t Velocitysd;
  uint8_t Minsatellites;
  uint8_t Timegain;
  uint8_t Maxlag;
  uint16_t Minsamples;
};
static_assert(sizeof(AugmentedSGP4ParametersMessageFormat) == 30);
template <> struct MessageSize<AugmentedSGP4ParametersMessageFormat> { static constexpr size_t value = 30; };
template <> struct SetID<AugmentedSGP4ParametersMessageFormat> { static constexpr int value = 28; };
template <> struct GetID<AugmentedSGP4ParametersMessageFormat> { static constexpr int value = 227; };

struct SDLog1ConfigurationMessageFormat {
  char LogSelection[10];
  uint16_t LogPeriod;
  SdLogSelectEnumerationValues LogDestination : 8;
};
static_assert(sizeof(SDLog1ConfigurationMessageFormat) == 13);
template <> struct MessageSize<SDLog1ConfigurationMessageFormat> { static constexpr size_t value = 13; };
template <> struct SetID<SDLog1ConfigurationMessageFormat> { static constexpr int value = 104; };
template <> struct GetID<SDLog1ConfigurationMessageFormat> { static constexpr int value = 235; };

struct SDLog2ConfigurationMessageFormat {
  char LogSelection[10];
  uint16_t LogPeriod;
  SdLogSelectEnumerationValues LogDestination : 8;
};
static_assert(sizeof(SDLog2ConfigurationMessageFormat) == 13);
template <> struct MessageSize<SDLog2ConfigurationMessageFormat> { static constexpr size_t value = 13; };
template <> struct SetID<SDLog2ConfigurationMessageFormat> { static constexpr int value = 105; };
template <> struct GetID<SDLog2ConfigurationMessageFormat> { static constexpr int value = 236; };

struct UARTLogConfigurationMessageFormat {
  char LogSelection[10];
  uint16_t LogPeriod;
};
static_assert(sizeof(UARTLogConfigurationMessageFormat) == 12);
template <> struct MessageSize<UARTLogConfigurationMessageFormat> { static constexpr size_t value = 12; };
template <> struct SetID<UARTLogConfigurationMessageFormat> { static constexpr int value = 106; };
template <> struct GetID<UARTLogConfigurationMessageFormat> { static constexpr int value = 237; };

} // namespace CubeSpace

#pragma pack(pop)

