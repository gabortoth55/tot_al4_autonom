# `tot_al4_autonom` package

Autonóm járművek és robotok programozása – kis beadandó. ROS 2 C++ package. [![Static Badge](https://img.shields.io/badge/ROS_2-Humble-34aec5)](https://docs.ros.org/en/humble/)

A package két node-ból áll:

- `battery_sim_node`: szimulált akkumulátort modellez. A töltöttség folyamatosan csökken (kis zajjal), az adatot `sensor_msgs/msg/BatteryState` típusú `/battery_state` topicban hirdeti. Lemerüléskor „újratölt”.
- `battery_monitor_node`: feliratkozik a `/battery_state` topicra, a töltöttség alapján állapotot (`OK`, `WARNING`, `CRITICAL`) állapít meg, és `std_msgs/msg/String` típusú `/battery_status` topicban hirdeti.

## Node-topic kapcsolat

```mermaid
graph LR
    A([/battery_sim_node]) -- "/battery_state<br/>sensor_msgs/BatteryState" --> B([/battery_monitor_node])
    B -- "/battery_status<br/>std_msgs/String" --> C[terminál / rqt / ros2 topic echo]
```

| Topic | Típus | Publisher | Subscriber |
|---|---|---|---|
| `/battery_state` | `sensor_msgs/msg/BatteryState` | `battery_sim_node` | `battery_monitor_node` |
| `/battery_status` | `std_msgs/msg/String` | `battery_monitor_node` | – |

## Paraméterek

| Node | Paraméter | Alapérték | Leírás |
|---|---|---|---|
| `battery_sim_node` | `publish_rate` | `1.0` | Publikálási frekvencia [Hz] |
| `battery_sim_node` | `discharge_per_s` | `2.0` | Merülési sebesség [%/s] |
| `battery_sim_node` | `full_voltage` / `empty_voltage` | `12.6` / `9.0` | Feszültség teli / üres állapotban [V] |
| `battery_monitor_node` | `warn_threshold` | `30.0` | `WARNING` küszöb [%] |
| `battery_monitor_node` | `critical_threshold` | `15.0` | `CRITICAL` küszöb [%] |

## Packages and build

Feltételezzük, hogy a workspace `~/ros2_ws/`, és a ROS 2 Humble telepítve van.

### Clone the packages

```bash
cd ~/ros2_ws/src
git clone https://github.com/gabortoth55/tot_al4_autonom
```

### Build ROS 2 packages

```bash
source /opt/ros/humble/setup.bash
cd ~/ros2_ws
colcon build --packages-select tot_al4_autonom --symlink-install
```

**Ne felejtsd el source-olni a ROS parancsok előtt.**

```bash
source ~/ros2_ws/install/setup.bash
```

## Futtatás

Launch fájllal (mindkét node egyszerre):

```bash
ros2 launch tot_al4_autonom battery.launch.py
```

Paraméterezve:

```bash
ros2 launch tot_al4_autonom battery.launch.py discharge_per_s:=5.0 warn_threshold:=40.0
```

Vagy külön terminálokban:

```bash
ros2 run tot_al4_autonom battery_sim_node
ros2 run tot_al4_autonom battery_monitor_node
```

## Ellenőrzés

```bash
ros2 topic list
ros2 topic echo /battery_state
ros2 topic echo /battery_status
rqt_graph
```

Várt kimenet a `/battery_status` topicon (a töltöttség csökkenésével):

```
data: 'OK: 80.3 % (11.89 V)'
...
data: 'WARNING: 28.1 % (10.01 V)'
...
data: 'CRITICAL: 13.9 % (9.50 V)'
```
