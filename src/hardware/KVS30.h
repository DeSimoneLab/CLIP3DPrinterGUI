#ifndef KVS30_h
#define KVS30_h

#include <stdint.h>
#include "serialib.h"

class KVS30 {
 public:
  typedef void( *FinishedListener )();
  //ASCII commands from KVS30 User Manual
  enum class CommandType {
    None,
    Acceleration,
    BacklashComp,
    HysterisisComp,
    DriverVoltage,
    KdLowPassFilterCutOff,
    FollowingErrorLim,
    FrictionComp,
    HomeSearchType,
    StageIdentifier,
    LeaveJoggingState,
    KeypadEnable,
    JerkTime,
    DerivativeGain,
    IntegralGain,
    ProportionalGain,
    VelocityFeedForward,
    Enable,
    HomeSearchVelocity,
    HomeSearch,
    HomeSearchTimeout,
    MoveAbs,
    MoveRel,
    MoveEstimate,
    Configure,
    Analog,
    TTLInputVal,
    Reset,
    RS485Adress,
    TTLOutputVal,
    ControlLoopState,
    NegativeSoftwareLim,
    PositiveSoftwareLim,
    StopMotion,
    EncoderIncrementVal,
    CommandErrorString,
    LastCommandErr,
    PositionAsSet,
    PositionReal,
    ErrorStatus,
    Velocity,
    BaseVelocity,
    ControllerRevisionInfo,
    AllConfigParam,
    ESPStageConfig,
  };
  enum class CommandParameterType {
    None,
    Int,
    Float,
  };
  enum class CommandGetSetType {
    None,
    Get,
    Set,
    GetSet,
    GetAlways,
  };
  enum class ModeType {
    Inactive,
    Idle,
    WaitingForCommandReply,
  };
  struct CommandStruct {
    CommandType Command;
    int CommandHeader[6];
//    CommandParameterType SendType;
//    CommandGetSetType GetSetType;
  };
  struct CommandEntry {
    const CommandStruct* Command;
    CommandGetSetType GetOrSet;
    float Parameter;

  };
  enum class StatusType {
    Unknown,
    Error,
    Config,
    NoReference,
    Homing,
    Moving,
    Ready,
    Disabled,
    Jogging,
  };
  struct StatusCharSet {
    const char* Code;
    StatusType Type;
  };
  bool KVS30Init(const char*);
  void KVS30Close();
  bool Home(void);
  bool QueryHardware();
  void SetVelocity(float VelocityToSet);
  void RelativeMove(float CommandParameter);
  void AbsoluteMove(float AbsoluteDistanceToMove);
  char* GetPosition();
  char* GetCustom(char* Command);
  char* GetVelocity();
  char* GetAcceleration();
  char* GetPositiveLimit();
  char* GetNegativeLimit();
  const char* GetError();
  char* GetMotionTime();
  void StopMotion();
  void SetPositiveLimit(float Limit);
  void SetNegativeLimit(float Limit);
  void SetAcceleration(float AccelerationToSet);
  void SetJerkTime(float JerkTime);
  int Available();
  serialib serial;

 private:
  static const char GetCharacter;
  //void Home(void);
  static const CommandStruct CommandLibrary[];
  static const StatusCharSet StatusLibrary[];
  const char* ConvertToErrorString(char ErrorCode);
  bool SendCurrentCommand();
  CommandEntry CommandToPrint;
  int *CommandHeader;
  const CommandStruct* CurrentCommand;
  CommandGetSetType CurrentCommandGetOrSet;
  float CurrentCommandParameter;
  void SetCommand(CommandType Type, float Parameter);
  StatusType ConvertStatus(char* StatusChar);
  char* SerialRead();
};

#endif
