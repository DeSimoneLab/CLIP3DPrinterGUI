#include "stagecommands.h"
#include <QTimer>

static float FeedRate = 60;

static byte accel[4] = {0x00, 0x00, 0x00, 0x00};
static byte vel[4] = {0x00, 0x00, 0x00, 0x00};

const float KVSVELSCALE = 447392.43;
const float KVSACCELSCALE = 152.71;
const float KVSPOSSCALE = 20000;

//static bool StagePrep1 = false;
//static bool StagePrep2 = false;
PrintSettings s_PrintSettings;
serialib StageSerial;

// Prints message to be sent over serial
void StageCommands::printSerialMessage(byte * message, int N)
{
    printf("\nsending message:\n");

    for (int i = 0; i < N; i++) {
        printf("0x%02x ", message[i]);
    }

    printf("\n\n");
}


/*!
 * \brief StageCommands::StageInit
 * Initializes the stage serial port connection.
 * \param COMPort - Select which COM port to connect to.
 * \param StageType - Select stage type between STAGE_SMC or STAGE_GCODE
 * \return - Returns 1 if successful connection, -1 if failed connection
 */
int StageCommands::StageInit(const char* COMPort, Stage_t StageType) {
    int returnVal = 0;

    switch(StageType) {

    case STAGE_SMC: {
        returnVal = SMC.SMC100CInit(COMPort);
    } break;

    case STAGE_KVS:
    {
        StageSerial.closeDevice();

        printf("Connecting to KVS stage on %s\n", COMPort);
        if(StageSerial.openDevice(COMPort, 115200) == 1) { //If serial connection was successful

            //returnVal = 1; //return 1 for successful connection
            printf("KVS stage connected\n");
            Sleep(10);

            int result;

            //            byte message1[6] = { 0x23, 0x02, 0x01, 0x00, 0x50, 0x01 }; // identify
            //            result = StageSerial.writeBytes(message1, 6);
            //            printf("Sent message1 / identify, result: %d\n", result);

            Sleep(10);

            printf("Flushed receiver: %X\n", StageSerial.flushReceiver());

            Sleep(10);

            // byte message2[6] = { 0x43, 0x04, 0x00, 0x00, 0x50, 0x01 }; // home
            byte message2[6] = { 0x05, 0x00, 0x00, 0x00, 0x50, 0x01 }; // req info
            result = StageSerial.writeBytes(message2, 6);
            printf("Sent message2, result: %d\n", result);

            Sleep(10);

            int N = 90;
            byte response[N];
            for (int i = 0; i < N; i++)   response[i] = 0;
            result = StageSerial.readBytes(response, N, 2000);

            printf("Read response, result: %d\n", result);

            for (int i = 0; i < 100; i++)   printf("%X ", response[i]);
            printf("\n");

            // Channel enable
            byte message3[6] = { 0x10, 0x02, 0x01, 0x01, 0x50, 0x01 }; // channel enable
            result = StageSerial.writeBytes(message3, 6);
            printf("Sent message3 / enable, result: %d", result);

            Sleep(100);

            // request for home
            byte message4[6] = { 0x43, 0x04, 0x01, 0x00, 0x50, 0x01 }; // home
            result = StageSerial.writeBytes(message4, 6);
            //printf("Sent message4 / home, result: %d", result);

            Sleep(100);

            returnVal = 1;
        }

        else { //Serial connection failed
            returnVal = -1;
            printf("KVS stage connection faileddd");
        }
    } break;

    case STAGE_GCODE: {
        if(StageSerial.openDevice(COMPort, 115200) == 1) { //If serial connection was successful
            returnVal = 1; //return 1 for successful connection
            Sleep(10); //Delay to avoid serial congestion

            //Set stage programming to mm
            QString UnitCommand = "\rG21\r\n";
            int UnitReturn = StageSerial.writeString(UnitCommand.toLatin1().data());
            if (UnitReturn < 0) //if command failed
                returnVal = -1; //return -1 for failed command
            Sleep(10); //Delay to avoid serial congestion

            //Set stage to use incremental movements
            QString IncrementCommand = "G91\r\n";
            int IncrementReturn = StageSerial.writeString(IncrementCommand.toLatin1().data());
            if (IncrementReturn < 0) //if command failed
                returnVal = -1; //return -1 for failed command
            Sleep(10); //Delay to avoid serial congestion

            //Enable steppers
            QString StepperCommand = "M17\r\n";
            int StepperReturn = StageSerial.writeString(StepperCommand.toLatin1().data());
            if (StepperReturn < 0) //if command failed
                returnVal = -1; //return -1 for failed command
            Sleep(500);
            StageSerial.flushReceiver();
            // TODO: can this be replaced by flushing the receiver??
            /*for (uint8_t i = 0; i < 25; i++){
          static char receivedString[] = "ThisIsMyTest";
          char finalChar = '\n';
          uint maxNbBytes = 100;//make sure to validate this
          int ReadStatus = StageSerial.readString(receivedString, finalChar, maxNbBytes, 10);
          Sleep(10);
      }*/
        } else { //Serial connection failed
            returnVal = -1;
            printf("GCode stage connection failed");
        }
    } break;

    }

    //    if(StageType == STAGE_SMC) {
    //        returnVal = SMC.SMC100CInit(COMPort);
    //    } else if (StageType == STAGE_GCODE) {
    //        if(StageSerial.openDevice(COMPort, 115200) == 1) { //If serial connection was successful
    //            returnVal = 1; //return 1 for successful connection
    //            Sleep(10); //Delay to avoid serial congestion

    //            //Set stage programming to mm
    //            QString UnitCommand = "\rG21\r\n";
    //            int UnitReturn = StageSerial.writeString(UnitCommand.toLatin1().data());
    //            if (UnitReturn < 0) //if command failed
    //                returnVal = -1; //return -1 for failed command
    //            Sleep(10); //Delay to avoid serial congestion

    //            //Set stage to use incremental movements
    //            QString IncrementCommand = "G91\r\n";
    //            int IncrementReturn = StageSerial.writeString(IncrementCommand.toLatin1().data());
    //            if (IncrementReturn < 0) //if command failed
    //                returnVal = -1; //return -1 for failed command
    //            Sleep(10); //Delay to avoid serial congestion

    //            //Enable steppers
    //            QString StepperCommand = "M17\r\n";
    //            int StepperReturn = StageSerial.writeString(StepperCommand.toLatin1().data());
    //            if (StepperReturn < 0) //if command failed
    //                returnVal = -1; //return -1 for failed command
    //            Sleep(500);
    //            StageSerial.flushReceiver();
    //            // TODO: can this be replaced by flushing the receiver??
    //            /*for (uint8_t i = 0; i < 25; i++){
    //          static char receivedString[] = "ThisIsMyTest";
    //          char finalChar = '\n';
    //          uint maxNbBytes = 100;//make sure to validate this
    //          int ReadStatus = StageSerial.readString(receivedString, finalChar, maxNbBytes, 10);
    //          Sleep(10);
    //      }*/
    //        } else { //Serial connection failed
    //            returnVal = -1;
    //            printf("GCode stage connection failed");
    //        }
    //    }
    if (returnVal == 1) {
        isConnected = true;
        emit StageConnect();
    }
    return returnVal;
}

/*!
 * \brief StageCommands::StageClose
 * Closes stage serial port connection.
 * \param StageType - Select stage type between STAGE_SMC or STAGE_GCODE
 * \return - Returns 1 if successful, -1 if failed
 */
int StageCommands::StageClose(Stage_t StageType) {
    int returnVal = 0;
    if(StageType == STAGE_SMC) {
        SMC.SMC100CClose();
    } else if (StageType == STAGE_GCODE) {
        StageSerial.closeDevice();
    }
    return returnVal;
}

/*!
 * \brief StageCommands::StageHome
 * Homes the stage to provide a reference point, only used for STAGE_SMC
 * \param StageType - Select stage type between STAGE_SMC or STAGE_GCODE
 * \return - Returns 1 if successful, -1 if failed
 */
int StageCommands::StageHome(Stage_t StageType) {
    int returnVal = 0;
    if(StageType == STAGE_SMC) {
        returnVal = SMC.Home();
    }
//    else if(StageType == STAGE_KVS) {
//        // request for home
//        byte message4[6] = { 0x43, 0x04, 0x01, 0x00, 0x50, 0x01 }; // home
//        int result = StageSerial.writeBytes(message4, 6);
//        printf("Sent message4 / home, result: %d", result);
//    }
    else if (StageType == STAGE_GCODE) {
        //TODO: is there a use for a stage home command for Gcode stage?
        returnVal = 1;
    }
    return returnVal;
}

/*!
 * \brief StageCommands::StageStop
 * \param StageType - Select stage type between STAGE_SMC or STAGE_GCODE
 * \return - Returns 1 if successful, -1 if failed
 */
int StageCommands::StageStop(Stage_t StageType) {
    int returnVal = 1; //default 1 is success
    if(StageType == STAGE_SMC) {
        SMC.StopMotion();
    } else if (StageType == STAGE_GCODE) {
        QString STOP = "M0\r";
        returnVal = StageSerial.writeString(STOP.toLatin1().data());
    } else if (StageType == STAGE_KVS) {

        int result;
        byte message2[6] = { 0x65, 0x04, 0x00, 0x01, 0x50, 0x01 }; // abrupt stop
        result = StageSerial.writeBytes(message2, 6);
        printf("Sent Request to Stop with %d bytes\n", result);

    }
    return returnVal;
}

/*!
 * \brief StageCommands::SetStageVelocity
 * \param VelocityToSet - Velocity to be set in unit of mm/s
 * \param StageType - Select stage type between STAGE_SMC or STAGE_GCODE
 * \return - Returns 1 if successful, -1 if failed
 */
int StageCommands::SetStageVelocity(float VelocityToSet, Stage_t StageType) {
    int returnVal = 0;
    if(StageType == STAGE_SMC) {
        SMC.SetVelocity(VelocityToSet);
    } else if (StageType == STAGE_GCODE) {
        /*For the Gcode stage the velocity is set along with the move command
      so we need to set the static FeedRate variable*/
        FeedRate = VelocityToSet * 60;
    } else if (StageType == STAGE_KVS) {

        unsigned long int Velocity = VelocityToSet * KVSVELSCALE;

        vel[3] = (byte) ((Velocity >> 24) & 0xFF);
        vel[2] = (byte) ((Velocity >> 16) & 0XFF);
        vel[1] = (byte) ((Velocity >> 8) & 0xFF);
        vel[0] = (byte) ((Velocity & 0xFF));



        int result;
        byte message[20] = { 0x13, 0x04, 0x0E, 0x00, 0x50 | 0x80, 0x01,     // set velocity header
                             0x01, 0x00,                             // channel identifier
                             0x00, 0x00, 0x00, 0x00,                 // min velocity (set to zero)
                             accel[0], accel[1], accel[2], accel[3], // acceleration bytes
                             vel[0], vel[1], vel[2], vel[3]          // velocity bytes
                           };

        result = StageSerial.writeBytes(message, 20);
        printf("Sent Velocity to: %.3f mm/s   %d\n", VelocityToSet, Velocity);
        StageCommands::printSerialMessage(message, 20);

    }
    return returnVal;
}

/*!
 * \brief StageCommands::SetStageAcceleration
 * \param AccelerationToSet - Acceleration to be in units of mm/s^2
 * \param StageType - Select stage type between STAGE_SMC or STAGE_GCODE
 * \return - Returns 1 if successful, -1 if failed
 */
int StageCommands::SetStageAcceleration(float AccelerationToSet, Stage_t StageType) {
    int returnVal = 0;
    if(StageType == STAGE_SMC) {
        SMC.SetAcceleration(AccelerationToSet);
    } else if (StageType == STAGE_GCODE) {
        /*QString AccelCommand = "M201 Z" + QString::number(AccelerationToSet) + "\r\n";
    int AccelReturn = StageSerial.writeString(AccelCommand.toLatin1().data());
    if (AccelReturn < 0)
        returnVal = -1;*/ // TODO: Check if accel works
    } else if (StageType == STAGE_KVS) {

        StageSerial.flushReceiver();

        unsigned long int Accel = AccelerationToSet * KVSACCELSCALE;

        accel[3] = (byte) ((Accel >> 24) & 0xFF);
        accel[2] = (byte) ((Accel >> 16) & 0XFF);
        accel[1] = (byte) ((Accel >> 8) & 0xFF);
        accel[0] = (byte) ((Accel & 0xFF));

        int result;
        byte message[20] = { 0x13, 0x04, 0x0E, 0x00, 0x50 | 0x80, 0x01,     // set accel header
                            0x01, 0x00,                             // channel identifier
                            0x00, 0x00, 0x00, 0x00,                 // min velocity (set to zero)
                            accel[0], accel[1], accel[2], accel[3], // acceleration bytes
                            vel[0], vel[1], vel[2], vel[3]          // velocity bytes
                         };


        result = StageSerial.writeBytes(message, 20);
        printf("Set Acceleration to: %.3f mm/s/s   %d\n", AccelerationToSet, Accel);
        StageCommands::printSerialMessage(message, 20);

    }
    return returnVal;
}

/*!
 * \brief StageCommands::SetStagePositiveLimit
 * \param PositiveLimit - Sets the positive stage endstop for SMC stage (mm)
 * \param StageType - Select stage type between STAGE_SMC or STAGE_GCODE
 * \return - Returns 1 if successful, -1 if failed
 */
int StageCommands::SetStagePositiveLimit(float PositiveLimit, Stage_t StageType) {
    int returnVal = 0;
    if(StageType == STAGE_SMC) {
        SMC.SetPositiveLimit(PositiveLimit);
    } else if (StageType == STAGE_GCODE) {
        // not applicable
    } else if (StageType == STAGE_KVS) {
        // todo
    }
    return returnVal;
}

/*!
 * \brief StageCommands::SetStageNegativeLimit
 * \param NegativeLimit - Sets the negative stage endstop for SMC stage (mm)
 * \param StageType - Select stage type between STAGE_SMC or STAGE_GCODE
 * \return - Returns 1 if successful, -1 if failed
 */
int StageCommands::SetStageNegativeLimit(float NegativeLimit, Stage_t StageType) {
    int returnVal = 0;
    if(StageType == STAGE_SMC) {
        SMC.SetNegativeLimit(NegativeLimit);
    } else if (StageType == STAGE_GCODE) {
        // not applicable
    } else if (StageType == STAGE_KVS) {
        // todo
    }
    return returnVal;
}

int StageCommands::SetStageJerkTime(float JerkTime, Stage_t StageType) {
    int returnVal = 0;
    if (StageType == STAGE_SMC) {
        SMC.SetJerkTime(JerkTime);
    }
    return returnVal;
}

/*!
 * \brief StageCommands::StageAbsoluteMove
 * \param AbsoluteMovePosition
 * \param StageType - Select stage type between STAGE_SMC or STAGE_GCODE
 * \return - Returns 1 if successful, -1 if failed
 */
int StageCommands::StageAbsoluteMove(float AbsoluteMovePosition, Stage_t StageType) {
    int returnVal = 0;
    if(StageType == STAGE_SMC) {
        SMC.AbsoluteMove(AbsoluteMovePosition);
    } else if (StageType == STAGE_GCODE) {
        //Set stage to do absolute move
        QString AbsMoveCommand = "G92\r\nG1 Z" + QString::number(AbsoluteMovePosition) + " F" + QString::number(FeedRate) + "\r\n";
        returnVal = StageSerial.writeString(AbsMoveCommand.toLatin1().data());
    } else if (StageType == STAGE_KVS) {

        StageSerial.flushReceiver();

        long int Position = AbsoluteMovePosition * KVSPOSSCALE;

        byte pos[4];

        pos[3] = (byte) ((Position >> 24) & 0xFF);
        pos[2] = (byte) ((Position >> 16) & 0XFF);
        pos[1] = (byte) ((Position >> 8) & 0xFF);
        pos[0] = (byte) ((Position & 0xFF));

        int result;
        byte message[12] = { 0x53, 0x04, 0x06, 0x00, 0x50 | 0x80, 0x01,     // set velocity header
                             0x01, 0x00,                                    // channel identifier
                             pos[0], pos[1], pos[2], pos[3]                 // position bytes
                         };

        StageCommands::printSerialMessage(message, 12);
        result = StageSerial.writeBytes(message, 12);
        printf("Sent position request with %d bytes\n", result);

    }
    return returnVal;
}

/*!
 * \brief StageCommands::StageRelativeMove
 * Moves the stage relative to it's current position.
 * \param RelativeMoveDistance - Distance to move in units of (mm)
 * \param StageType - Select stage type between STAGE_SMC or STAGE_GCODE
 * \return - Returns 1 if successful, -1 if failed
 */
int StageCommands::StageRelativeMove(float RelativeMoveDistance, Stage_t StageType) {
    int returnVal = 0;
    if(StageType == STAGE_SMC) {
        SMC.RelativeMove(RelativeMoveDistance);
    } else if (StageType == STAGE_GCODE) {
        QString RelMoveCommand = "G91\r\nG1 Z" + QString::number(-RelativeMoveDistance) + " F" + QString::number(FeedRate) + "\r\n";
        returnVal = StageSerial.writeString(RelMoveCommand.toLatin1().data());
    } else if (StageType == STAGE_KVS) {

        StageSerial.flushReceiver();

        long int RelPos = RelativeMoveDistance * KVSPOSSCALE;

        byte relpos[4];

        relpos[3] = (byte) ((RelPos >> 24) & 0xFF);
        relpos[2] = (byte) ((RelPos >> 16) & 0XFF);
        relpos[1] = (byte) ((RelPos >> 8) & 0xFF);
        relpos[0] = (byte) ((RelPos & 0xFF));

        int result;
        byte message[12] = { 0x48, 0x04, 0x06, 0x00, 0x50 | 0x80, 0x01,     // set velocity header
                             0x01, 0x00,                                    // channel identifier
                             relpos[0], relpos[1], relpos[2], relpos[3]     // relative move distance
                         };

        StageCommands::printSerialMessage(message, 12);

        result = StageSerial.writeBytes(message, 12);
        printf("Relative Move: %d mm \t%d\n", result);

    }
    return returnVal;
}

/*!
 * \brief StageCommands::StageGetPosition
 * Gets current stage position.
 * \param StageType - Select stage type between STAGE_SMC or STAGE_GCODE
 * \return - Returns QString containing the current stage position
 */
QString StageCommands::StageGetPosition(Stage_t StageType) {
    if(StageType == STAGE_SMC) {
        char* ReadPosition = SMC.GetPosition();
        if (strnlen(ReadPosition, 50) > 1) {
            QString CurrentPosition = QString::fromUtf8(ReadPosition);
            CurrentPosition.remove(0, 3); //Removes address and command
            CurrentPosition.chop(2);
            return CurrentPosition;
        }
    } else if (StageType == STAGE_GCODE) { //Test if
        QString GetPositionCommand = "M114 R\r\n";
        StageSerial.flushReceiver(); //Flush receiver to get rid of readout we don't need
        StageSerial.writeString(GetPositionCommand.toLatin1().data());
        static char receivedString[] = "ThisIsMyPositionTest";
        char finalChar = '\n';
        uint maxNbBytes = 100;//make sure to validate this
        StageSerial.readString(receivedString, finalChar, maxNbBytes, 10);
        printf(receivedString);
        QString CurrentPosition = QString::fromUtf8(receivedString);
        QString returnVal = CurrentPosition.mid(CurrentPosition.indexOf("Z"),
                                                CurrentPosition.indexOf("E"));
        returnVal.remove(0, 2);
        returnVal.chop(3);

        return returnVal;

    } else if (StageType == STAGE_KVS) {
        byte counter[4];

        StageSerial.flushReceiver();

        byte message[6] = { 0x11, 0x04, 0x01, 0x00, 0x50, 0x01 };

        StageSerial.writeBytes(message, 6);


        int N = 12;
        byte response[N];
        for (int i = 0; i < N; i++)   response[i] = 0;
        StageSerial.readBytes(response, N, 2000);

        for (int i = 0; i < N; i++)   printf("%X ", response[i]);
        printf("\n");

        counter[3] = response[8];
        counter[2] = response[9];
        counter[1] = response[10];
        counter[0] = response[11];

        long int Counter = (counter[0] << 24) +
                                    (counter[1] << 16) +
                                    (counter[2] << 8) +
                                    (counter[3]);

        float Position = Counter / 20000.0;


        //float Position = Counter / KVSPOSSCALE;

        printf("Current Position: %.3f\n", Position);

        QString returnVal = QString::number(Position);

        return returnVal;
    }
    return "NA";
}

int StageCommands::SendCustom(Stage_t StageType, QString Command) {
    if (StageType == STAGE_SMC) {
        //char* command = qPrintable(Command.toStdString());
        //char* ReadPosition = SMC.GetCustom(Command.toLocal8bit.constData());
    } else if (StageType == STAGE_GCODE) {
        return StageSerial.writeString(Command.toLatin1().data());
    }
    return 0;
}
/****************************Helper Functions****************************/
/*!
 * \brief StageCommands::initStagePosition
 * Saves the print settings to a module variable
 * and calls initStageSlot
 * \param si_PrintSettings - Print settings passed down from the Main Window
 */
void StageCommands::initStagePosition(PrintSettings si_PrintSettings) {
    s_PrintSettings = si_PrintSettings;
    initStageSlot();
    StageSerial.flushReceiver();
}

static int MovementAttempts = 0;
/*!
 * \brief StageCommands::initStageSlot
 * Starts process of lowering stage down to it's starting position,
 * tells stage to move to 3.2mm above the starting position then checks
 * the stage position once per second and once it has reached the position
 * 3.2mm above the starting position calls the fineMovement function
 */
void StageCommands::initStageSlot() {
    if (s_PrintSettings.StageType == STAGE_SMC) {
        double CurrentPosition = StageGetPosition(STAGE_SMC).toDouble();
        emit StageGetPositionSignal(QString::number(CurrentPosition));
        if (CurrentPosition < (s_PrintSettings.StartingPosition - 3.2)) {
            SetStageAcceleration(3, s_PrintSettings.StageType);
            SetStageVelocity(3, s_PrintSettings.StageType);
            Sleep(20);
            StageAbsoluteMove(s_PrintSettings.StartingPosition - 3, s_PrintSettings.StageType);
            emit StagePrintSignal("Performing Rough Stage Move to: " + QString::number(s_PrintSettings.StartingPosition - 3));
            repeatRoughSlot();
        } else {
            fineMovement();
        }
    } else {
        emit StagePrintSignal("Auto stage initialization disabled for iCLIP, please move stage to endstop with manual controls");
    }
}

/*!
 * \brief StageCommands::fineMovement
 * Sets the stage velocity to 0.3 mm/s and lowers the stage
 * the final 3.2 mm down to the starting position, this is
 * done to reduce the bubble created when the stage enters
 * the resin
 */
void StageCommands::fineMovement() {
    double CurrentPosition = StageGetPosition(STAGE_SMC).toDouble();
    emit StageGetPositionSignal(QString::number(CurrentPosition));
    MovementAttempts = 0;
    //If stage has not reached it's final starting position
    if (CurrentPosition > s_PrintSettings.StartingPosition - 0.05 && CurrentPosition < s_PrintSettings.StartingPosition + 0.05) {
        StageAbsoluteMove(s_PrintSettings.StartingPosition, s_PrintSettings.StageType);
        verifyStageParams(s_PrintSettings);
    } else {
        SetStageAcceleration(0.3, s_PrintSettings.StageType);
        SetStageVelocity(0.3, s_PrintSettings.StageType);
        Sleep(20);
        StageAbsoluteMove(s_PrintSettings.StartingPosition, s_PrintSettings.StageType);
        emit StagePrintSignal("Fine Stage Movement to: " + QString::number(s_PrintSettings.StartingPosition));
        repeatFineSlot();
    }
}

void StageCommands::repeatRoughSlot() {
    double CurrentPosition = StageGetPosition(STAGE_SMC).toDouble();
    emit StageGetPositionSignal(QString::number(CurrentPosition));
    if (CurrentPosition > s_PrintSettings.StartingPosition - 3.2 && CurrentPosition < s_PrintSettings.StartingPosition + 3.2) {
        fineMovement();
    } else {
        if (MovementAttempts < 30) {
            QTimer::singleShot(1000, this, SLOT(repeatRoughSlot()));
        } else {
            emit StagePrintSignal("Could not move stage to starting position");
        }
    }

}

void StageCommands::repeatFineSlot() {
    double CurrentPosition = StageGetPosition(STAGE_SMC).toDouble();
    emit StageGetPositionSignal(QString::number(CurrentPosition));
    if (CurrentPosition > s_PrintSettings.StartingPosition - 0.05 && CurrentPosition < s_PrintSettings.StartingPosition + 0.05) {
        StageAbsoluteMove(s_PrintSettings.StartingPosition, s_PrintSettings.StageType);
        verifyStageParams(s_PrintSettings);
    } else {
        if (MovementAttempts < 30) {
            QTimer::singleShot(1000, this, SLOT(repeatFineSlot()));
        } else {
            emit StagePrintSignal("Could not move stage to starting position");
        }
    }
}

/*!
 * \brief StageCommands::verifyStageParams
 * After the stage has reached it's final position
 * and is ready to start the final stage printing parameters
 * are sent to the stage.
 * \param s_PrintSettings - Print settings passed down from MainWindow,
 * uses StageType, StageVelocity, StageAcceleration, MinEndOfRun, and StageVelocity
 */
void StageCommands::verifyStageParams(PrintSettings s_PrintSettings) {
    emit StagePrintSignal("Verifying Stage Parameters");
    SetStageJerkTime(s_PrintSettings.JerkTime, s_PrintSettings.StageType);
    Sleep(20);
    SetStageAcceleration(s_PrintSettings.StageAcceleration, s_PrintSettings.StageType);
    Sleep(20);
    SetStageNegativeLimit(s_PrintSettings.MinEndOfRun, s_PrintSettings.StageType);
    Sleep(20);
    SetStagePositiveLimit(s_PrintSettings.MaxEndOfRun, s_PrintSettings.StageType);
    Sleep(20);
    SetStageVelocity(s_PrintSettings.StageVelocity, s_PrintSettings.StageType);
    Sleep(20);
    double StagePosition  = StageGetPosition(STAGE_SMC).toDouble();
    emit StageGetPositionSignal(QString::number(StagePosition));
}

/*!
 * \brief StageCommands::initStageStart
 * Used to confirm that the stage velocity is set correctly,
 * if in Continuous motion mode calculates the continuous stage
 * velocity and sets it.
 * \param si_PrintSettings Print settings passed down from MainWindow,
 * uses StageType, MotionMode, StageVelocity, and ExposureTime
 */
void StageCommands::initStageStart(PrintSettings si_PrintSettings) {
    if(si_PrintSettings.MotionMode == STEPPED) {
        SetStageVelocity(si_PrintSettings.StageVelocity, si_PrintSettings.StageType);
        SetStageAcceleration(si_PrintSettings.StageAcceleration, si_PrintSettings.StageType);
    } else if (si_PrintSettings.MotionMode == CONTINUOUS) {
        double ContStageVelocity = (si_PrintSettings.StageVelocity) / (si_PrintSettings.ExposureTime / (1e6)); //Multiply Exposure time by 1e6 to convert from us to s to get to proper units
        emit StagePrintSignal("Continuous Stage Velocity set to " + QString::number(si_PrintSettings.StageVelocity) + "/" + QString::number(si_PrintSettings.ExposureTime) + " = " + QString::number(ContStageVelocity) + " mm/s");
        SetStageVelocity(ContStageVelocity, si_PrintSettings.StageType);
        SetStageAcceleration(si_PrintSettings.StageAcceleration, si_PrintSettings.StageType);
    }
    Sleep(10);
}
/*
void StageCommands::StageAbort(PrintSettings si_PrintSettings)
{
    StageStop(si_PrintSettings.StageType);
}
*/
void StageCommands::resetStageInit() {
    MovementAttempts = 0;
}
