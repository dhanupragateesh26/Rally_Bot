# 🤖 ESP32 CRSF Differential Drive Robot

A **high-performance radio-controlled differential drive robot** built using **ESP32 and ExpressLRS (CRSF protocol)**.

The system receives **low-latency control signals from an ExpressLRS receiver**, processes them using **expo curves, smoothing, and dynamic steering weight**, and drives two motors using a **differential drive control algorithm**.

Designed for:

* RC robots
* combat robots
* autonomous robotics platforms
* robotics research

---

# 🚀 Features

* ExpressLRS **CRSF protocol support**
* Ultra-low latency **420000 baud communication**
* Differential drive motor control
* **Dynamic steering weight tuning from transmitter**
* **Expo curves for smooth control**
* Motor acceleration smoothing
* Fail-safe motor shutdown
* Deadband filtering
* Minimum PWM compensation for motor friction

---

# ⚙️ Hardware Used

| Component           | Purpose             |
| ------------------- | ------------------- |
| ESP32               | Main controller     |
| ExpressLRS Receiver | Radio control input |
| Motor Drivers       | Motor control       |
| DC Motors           | Robot movement      |
| LiPo Battery        | Power supply        |

---

# 📡 Communication Protocol

The system uses the **CRSF (Crossfire) protocol** used by ExpressLRS.

Characteristics:

* High speed communication
* Low latency
* Multiple channel data
* Telemetry support

Communication speed used:

```
420000 baud
```

---

# 🎮 Channel Mapping

| Channel | Function               |
| ------- | ---------------------- |
| CH1     | Steering               |
| CH2     | Throttle               |
| CH5     | Arm / Safety switch    |
| CH10    | Steering weight tuning |

---

# 🧠 Control Algorithm

The robot uses **differential drive mixing**.

```
LeftSpeed  = Throttle + (Steering × Weight)
RightSpeed = Throttle − (Steering × Weight)
```

Where:

* **Throttle** controls forward/backward movement
* **Steering** controls turning
* **Weight** adjusts steering aggressiveness

---

# 🎚 Dynamic Steering Control

Steering sensitivity can be adjusted **live from the transmitter** using a slider or scroll wheel.

```
Weight = (ChannelValue - 1000) / 1000
```

Range:

```
0.1 → 1.0
```

This allows the driver to:

* Reduce steering sensitivity at high speeds
* Increase turning sharpness for tight maneuvering

---

# 📉 Input Processing Pipeline

Control signals pass through several stages:

### 1️⃣ Deadband

Small stick movements are ignored.

```
DEADBAND = 5%
```

### 2️⃣ Expo Curve

Improves precision near the center.

```
output = input*(1-expo) + input³*(expo)
```

### 3️⃣ Smoothing

Gradually ramps motor speed.

```
current = start + (target-start)*smoothFactor
```

### 4️⃣ Motor Output

Converted to PWM signals.

---

# 🛡 Fail-Safe System

Motors automatically stop if:

* Radio link is lost
* Arm switch is disabled

```
if (!crsf.isLinkUp() || armSwitch < 1500)
```

This prevents runaway robots.

---

# 📂 Firmware

```
firmware/
└── esp32_crsf_drive.ino
```

Contains:

* CRSF communication
* input filtering
* control algorithm
* motor driver control

---

# 🔌 Pin Configuration

| Function    | ESP32 Pin |
| ----------- | --------- |
| Receiver RX | 16        |
| Receiver TX | 17        |

### Left Motor

| Pin | Function    |
| --- | ----------- |
| 32  | Forward PWM |
| 33  | Reverse PWM |

### Right Motor

| Pin | Function    |
| --- | ----------- |
| 26  | Forward PWM |
| 27  | Reverse PWM |

---

# ▶️ How to Run

1️⃣ Install required libraries

* AlfredoCRSF

2️⃣ Upload firmware to ESP32.

3️⃣ Connect ExpressLRS receiver.

4️⃣ Arm using transmitter switch.

The robot will respond to transmitter inputs.

---

# 🎥 Demo

Add demo here once available.

```
media/demo.gif
```

---

# 🔧 Future Improvements

* Closed loop speed control
* IMU stabilization
* autonomous navigation mode
* ROS2 integration
* telemetry feedback to transmitter

---

# 👨‍💻 Author

Dhanu Pragateesh
Robotics Developer
Chennai, India
