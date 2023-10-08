#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <cstdint>

using namespace std;

// Enumerators.
enum class MessageType { SI_MESSAGE = 1, EI_MESSAGE = 2, SUMMARY_MESSAGE = 3 };

// Structures.
struct SatelliteInfo
{
    float temperature;
    float voltage;
};

struct ExperimentInfo
{
    uint16_t radiationCount;
    uint16_t latchupCount;
};

// Union.
union MasterUnion
{
    SatelliteInfo satelliteInfo;
    ExperimentInfo experimentInfo;
};

// Structure.
struct MasterInfo
{
    MessageType messageType;
    MasterUnion masterUnion;
};

// Constants.
const string INPUT_CHANNEL = "inputDataAntenna.txt";
const uint16_t STORAGE_SIZE = 5;

int main()
{
    // Variable declarations.
    ifstream inputAntennaReceiverChannel;
    MasterInfo masterInfoArr[STORAGE_SIZE];
    uint8_t entryIndex = 0;
    MasterInfo masterInfo;

    // Receiving Satellite Data.
    inputAntennaReceiverChannel.open(INPUT_CHANNEL);

    // Verifying that the receiving channel is working.
    if (inputAntennaReceiverChannel)
    {
        // Setting all elements of Master Info Array to empty info type.
        for (int elem = 0; elem < STORAGE_SIZE; elem++)
        {
            masterInfoArr[elem].messageType = MessageType::SUMMARY_MESSAGE;
        }

        // Getting message type.
        uint16_t messageId;

        // While receiving messageIDs.
        while (inputAntennaReceiverChannel >> messageId)
        {
            MessageType messageType = static_cast<MessageType>(messageId);

            // Storing data according to messageID.
            if (messageType == MessageType::SI_MESSAGE)
            {
                // Setting information type.
                masterInfo.messageType = MessageType::SI_MESSAGE;

                // Getting Temperature and voltage.
                inputAntennaReceiverChannel
                    >> masterInfo.masterUnion.satelliteInfo.temperature
                    >> masterInfo.masterUnion.satelliteInfo.voltage;

                // Storing data into array.
                masterInfoArr[entryIndex] = masterInfo;

                // Incrementing entry index (until 5th element).
                entryIndex = (entryIndex + 1) % STORAGE_SIZE;
            }
            else if (messageType == MessageType::EI_MESSAGE)
            {
                // Setting information type.
                masterInfo.messageType = MessageType::EI_MESSAGE;

                // Getting Radiation Count and Latchup Count.
                inputAntennaReceiverChannel
                    >> masterInfo.masterUnion.experimentInfo.radiationCount
                    >> masterInfo.masterUnion.experimentInfo.latchupCount;

                // Storing data into array.
                masterInfoArr[entryIndex] = masterInfo;

                // Incrementing entry index (until 5th element).
                entryIndex = (entryIndex + 1) % STORAGE_SIZE;
            }
            else if (messageType == MessageType::SUMMARY_MESSAGE)
            {
                // Variable declarations.
                float maxTemp = -50.0, minTemp = 212.0;
                float maxVolt = 0.0, minVolt = 100.0;
                uint16_t countSI = 0, countEI = 0, totalRad = 0, totalLatchup = 0;

                // Send info and calculate for summary.
                for (uint8_t elem = 0; elem < STORAGE_SIZE; elem++)
                {
                    if (masterInfoArr[elem].messageType == MessageType::SI_MESSAGE)
                    {
                        // Satellite Information.
                        cout << "Temperature: " << masterInfoArr[elem].masterUnion.satelliteInfo.temperature << endl;
                        cout << "Voltage: " << masterInfoArr[elem].masterUnion.satelliteInfo.voltage << endl << endl;

                        countSI++;

                        // Calculation for summary.
                        if (maxTemp < masterInfoArr[elem].masterUnion.satelliteInfo.temperature)
                        {
                            maxTemp = masterInfoArr[elem].masterUnion.satelliteInfo.temperature;
                        }
                        if (minTemp > masterInfoArr[elem].masterUnion.satelliteInfo.temperature)
                        {
                            minTemp = masterInfoArr[elem].masterUnion.satelliteInfo.temperature;
                        }

                        if (maxVolt < masterInfoArr[elem].masterUnion.satelliteInfo.voltage)
                        {
                            maxVolt = masterInfoArr[elem].masterUnion.satelliteInfo.voltage;
                        }
                        if (minVolt > masterInfoArr[elem].masterUnion.satelliteInfo.voltage)
                        {
                            minVolt = masterInfoArr[elem].masterUnion.satelliteInfo.voltage;
                        }
                    }
                    else if (masterInfoArr[elem].messageType == MessageType::EI_MESSAGE)
                    {
                        // Experiment Information.
                        cout << "Radiation Count: " << masterInfoArr[elem].masterUnion.experimentInfo.radiationCount << endl;
                        cout << "Latchup Event Count: " << masterInfoArr[elem].masterUnion.experimentInfo.latchupCount << endl << endl;

                        countEI++;

                        // Calculation for summary.
                        totalRad += masterInfoArr[elem].masterUnion.experimentInfo.radiationCount;
                        totalLatchup += masterInfoArr[elem].masterUnion.experimentInfo.latchupCount;
                    }
                }

                // Output of Summary.
                cout << "Summary Information" << endl;
                cout << "-------------------" << endl;
                cout << "Number of Satellite Information Records: " << countSI << endl;
                cout << "Number of Experiment Information Records: " << countEI << endl;
                cout << "Total Radiation Count: " << totalRad << endl;
                cout << "Total Latchup Event Count: " << totalLatchup << endl;
                cout << "Maximum Temperature: " << maxTemp << endl;
                cout << "Minimum Temperature: " << minTemp << endl;
                cout << "Maximum Voltage: " << maxVolt << endl;
                cout << "Minimum Voltage: " << minVolt << endl << endl;

                // Pause.
                cout << "Press the enter key to continue..." << endl;
                cin.ignore();
                cin.get();
            }
        }
    }
    else
    {
        cout << "No data received" << endl;
    }

    return 0;
}
