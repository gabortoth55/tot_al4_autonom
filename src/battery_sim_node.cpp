// Szimulalt akkumulator: lassan merulo toltottseg, BatteryState uzenettel.
#include <chrono>
#include <functional>
#include <memory>
#include <random>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/battery_state.hpp"

class BatterySimNode : public rclcpp::Node
{
public:
  BatterySimNode()
  : Node("battery_sim_node"),
    level_(100.0),
    gen_(std::random_device{}()),
    noise_(0.8, 1.2)
  {
    const double rate = declare_parameter<double>("publish_rate", 1.0);       // Hz
    discharge_per_s_ = declare_parameter<double>("discharge_per_s", 2.0);     // %/s
    full_voltage_ = declare_parameter<double>("full_voltage", 12.6);          // V
    empty_voltage_ = declare_parameter<double>("empty_voltage", 9.0);         // V
    dt_ = 1.0 / rate;

    pub_ = create_publisher<sensor_msgs::msg::BatteryState>("battery_state", 10);
    timer_ = create_wall_timer(
      std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(dt_)),
      std::bind(&BatterySimNode::timer_callback, this));

    RCLCPP_INFO(get_logger(), "battery_sim_node elindult");
  }

private:
  void timer_callback()
  {
    // Merules zajjal; ha lemerul, "ujratoltjuk"
    level_ -= discharge_per_s_ * dt_ * noise_(gen_);
    if (level_ <= 0.0) {
      RCLCPP_INFO(get_logger(), "Akkumulator lemerult, ujratoltes (100 %%)");
      level_ = 100.0;
    }

    sensor_msgs::msg::BatteryState msg;
    msg.header.stamp = now();
    msg.header.frame_id = "battery";
    msg.percentage = static_cast<float>(level_ / 100.0);  // sensor_msgs: 0.0 - 1.0
    msg.voltage = static_cast<float>(
      empty_voltage_ + (full_voltage_ - empty_voltage_) * (level_ / 100.0));
    msg.present = true;
    msg.power_supply_status = sensor_msgs::msg::BatteryState::POWER_SUPPLY_STATUS_DISCHARGING;
    pub_->publish(msg);

    RCLCPP_INFO(get_logger(), "Toltottseg: %5.1f %%  Feszultseg: %.2f V", level_, msg.voltage);
  }

  double level_;
  double dt_;
  double discharge_per_s_;
  double full_voltage_;
  double empty_voltage_;
  std::mt19937 gen_;
  std::uniform_real_distribution<double> noise_;
  rclcpp::Publisher<sensor_msgs::msg::BatteryState>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<BatterySimNode>());
  rclcpp::shutdown();
  return 0;
}
