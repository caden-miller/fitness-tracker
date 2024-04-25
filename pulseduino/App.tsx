import React, { useState, useEffect } from "react";
import {
  SafeAreaView,
  StyleSheet,
  Text,
  TouchableOpacity,
  View,
  Dimensions,
} from "react-native";
import DeviceModal from "./DeviceConnectionModal";
import { PulseIndicator } from "./PulseIndicator";
import useBLE from "./useBLE";
import AsyncStorage from "@react-native-async-storage/async-storage";
import { LineChart } from "react-native-chart-kit";

const App = () => {
  const {
    requestPermissions,
    scanForPeripherals,
    allDevices,
    connectToDevice,
    connectedDevice,
    heartRate,
    disconnectFromDevice,
  } = useBLE();
  const [isModalVisible, setIsModalVisible] = useState<boolean>(false);

  const scanForDevices = async () => {
    const isPermissionsEnabled = await requestPermissions();
    if (isPermissionsEnabled) {
      scanForPeripherals();
    }
  };

  const hideModal = () => {
    setIsModalVisible(false);
  };

  const openModal = async () => {
    scanForDevices();
    setIsModalVisible(true);
  };

  const [heartRateData, setHeartRateData] = useState<number[]>([]);
  const [averageHeartRate, setAverageHeartRate] = useState(0);

  useEffect(() => {
    // load heart rate data from local storage when component mounts
    AsyncStorage.getItem("heartRateData").then((data) => {
      if (data) {
        setHeartRateData(JSON.parse(data));
      }
    });
  }, []);

  useEffect(() => {
    // save heart rate data to local storage whenever it changes
    AsyncStorage.setItem("heartRateData", JSON.stringify(heartRateData))
      .catch(async (error) => {
        // if storage is full, remove the oldest heart rate reading and try again
        if (error.message.includes("database or disk is full")) {
          const newData = heartRateData.slice(1);
          setHeartRateData(newData);
          await AsyncStorage.setItem("heartRateData", JSON.stringify(newData));
        }
      });
  }, [heartRateData]);

  useEffect(() => {
    // update heart rate data whenever a new heart rate is received
    if (heartRate) {
      setHeartRateData((prevData) => [...prevData, heartRate]);
    }
  }, [heartRate]);

  useEffect(() => {
    // calculate average heart rate whenever heart rate data changes
    if (heartRateData.length > 0) {
      const sum = heartRateData.reduce((a, b) => a + b, 0);
      const avg = sum / heartRateData.length;
      setAverageHeartRate(avg);
    }
  }, [heartRateData]);

  return (
    <SafeAreaView style={styles.container}>
      <View style={styles.heartRateTitleWrapper}>
        {connectedDevice ? (
          <>
            <PulseIndicator />
            <Text style={styles.heartRateTitleText}>Your Heart Rate Is:</Text>
            <Text style={styles.heartRateText}>{heartRate} bpm</Text>
            {heartRateData.length > 0 && (
          <LineChart
            data={{
              labels: heartRateData.map((_, i) => i.toString()),
              datasets: [
                {
                  data: heartRateData,
                  color: (opacity = 1) => `rgba(134, 65, 244, ${opacity})`,
                  strokeWidth: 2 
                },
                {
                  data: heartRateData.map(() => averageHeartRate),
                  color: (opacity = 1) => `rgba(0, 255, 0, ${opacity})`,
                  strokeWidth: 2
                },
              ],
            }}
            width={Dimensions.get("window").width} // from react-native
            height={220}
            yAxisLabel=""
            yAxisSuffix=" bpm"
            yAxisInterval={1}
            chartConfig={{
              backgroundColor: "#e26a00",
              backgroundGradientFrom: "#fb8c00",
              backgroundGradientTo: "#ffa726",
              decimalPlaces: 2,
              color: (opacity = 1) => `rgba(255, 255, 255, ${opacity})`,
              labelColor: (opacity = 1) => `rgba(255, 255, 255, ${opacity})`,
              style: {
                borderRadius: 16,
              },
              propsForDots: {
                r: "6",
                strokeWidth: "2",
                stroke: "#ffa726",
              },
            }}
            bezier
            style={{
              marginVertical: 8,
              borderRadius: 16,
            }}
          />
          )}
        </>
        ) : (
          <Text style={styles.heartRateTitleText}>
            Please Connect to a Heart Rate Monitor
          </Text>
        )}
      </View>
      <TouchableOpacity
        onPress={connectedDevice ? disconnectFromDevice : openModal}
        style={styles.ctaButton}
      >
        <Text style={styles.ctaButtonText}>
          {connectedDevice ? "Disconnect from Device" : "Connect to Device"}
        </Text>
      </TouchableOpacity>
      <DeviceModal
        closeModal={hideModal}
        visible={isModalVisible}
        connectToPeripheral={connectToDevice}
        devices={allDevices}
      />
    </SafeAreaView>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#f2f2f2",
  },
  heartRateTitleWrapper: {
    flex: 1,
    justifyContent: "center",
    alignItems: "center",
  },
  heartRateTitleText: {
    fontSize: 30,
    fontWeight: "bold",
    textAlign: "center",
    marginHorizontal: 20,
    color: "black",
  },
  heartRateText: {
    fontSize: 25,
    marginTop: 15,
  },
  ctaButton: {
    backgroundColor: "#FF6060",
    justifyContent: "center",
    alignItems: "center",
    height: 50,
    marginHorizontal: 20,
    marginBottom: 5,
    borderRadius: 8,
  },
  ctaButtonText: {
    fontSize: 18,
    fontWeight: "bold",
    color: "white",
  },
});

export default App;