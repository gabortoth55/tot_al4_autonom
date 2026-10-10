// Akkumulator felugyelet: BatteryState figyeles, allapot kiirasa String topicban.
#include <cstdio>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/battery_state.hpp"
#include "std_msgs/msg/string.hpp"

class BatteryMonitorNode : public rclcpp::Node
{
public:
  BatteryMonitorNode()
  : Node("battery_monitor_node")
  {
    warn_ = declare_parameter<double>("warn_threshold", 30.0);          // %
    critical_ = declare_parameter<double>("critical_threshold", 15.0);  // %

    sub_ = create_subscription<sensor_msgs::msg::BatteryState>(
      "battery_state", 10,
      std::bind(&BatteryMonitorNode::listener_callback, this, std::placeholders::_1));
    pub_ = create_publisher<std_msgs::msg::String>("battery_status", 10);

    RCLCPP_INFO(get_logger(), "battery_monitor_node elindult");
  }

private:
  void listener_callback(const sensor_msgs::msg::BatteryState::SharedPtr msg)
  {
    const double percent = msg->percentage * 100.0;

    std::string state;
    if (percent <= critical_) {
      state = "CRITICAL";
    } else if (percent <= warn_) {
      state = "WARNING";
    } else {
      state = "OK";
    }

    char buf[96];
    std::snprintf(buf, sizeof(buf), "%s: %.1f %% (%.2f V)", state.c_str(), percent, msg->voltage);

    std_msgs::msg::String out;
    out.data = buf;
    pub_->publish(out);

    // Csak allapotvaltaskor naplozunk, hogy ne legyen zajos a terminal
    if (state != last_state_) {
      if (state == "CRITICAL") {
        RCLCPP_ERROR(get_logger(), "%s", buf);
      } else if (state == "WARNING") {
        RCLCPP_WARN(get_logger(), "%s", buf);
      } else {
        RCLCPP_INFO(get_logger(), "%s", buf);
      }
      last_state_ = state;
    }
  }

  double warn_;
  double critical_;
  std::string last_state_;
  rclcpp::Subscription<sensor_msgs::msg::BatteryState>::SharedPtr sub_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<BatteryMonitorNode>());
  rclcpp::shutdown();
  return 0;
}
