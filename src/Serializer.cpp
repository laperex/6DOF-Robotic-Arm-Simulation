#include "Serializer.h"
#include "ToYaml.h"
#include "ECS.h"
#include <LucyUtil/LucyUtil.h>
#include "RoboticArm.h"

static auto& registry = Registry::Instance();

void SerializeRoboticArmAnimations(std::string filename) {
	auto& roboticarm = registry.store<RoboticArm>();

	YAML::Emitter out;

	out << YAML::BeginSeq;

	for (auto& pair: roboticarm.animation_registry) {
		out << YAML::BeginMap;

		out << YAML::Key << "name";
		out << YAML::Value << pair.second.name;
		out << YAML::Key << "id";
		out << YAML::Value << pair.first;
		out << YAML::Key << "loop";
		out << YAML::Value << pair.second.animation.loop;
		out << YAML::Key << "delay";
		out << YAML::Value << pair.second.animation.delay;
		out << YAML::Key << "positions";
		out << YAML::Value;

		{
			out << YAML::BeginSeq;

			for (auto& step: pair.second.animation.animation_step_array) {
				out << YAML::BeginMap;

				out << YAML::Key << "progress_len";
				out << YAML::Value << step.progress_len;
				out << YAML::Key << "pow_t";
				out << YAML::Value << step.pow_t;

				out << YAML::Key << "base";
				out << YAML::Value << step.position.base;
				out << YAML::Key << "lower_elbow";
				out << YAML::Value << step.position.lower_elbow;
				out << YAML::Key << "upper_elbow";
				out << YAML::Value << step.position.upper_elbow;
				out << YAML::Key << "wrist";
				out << YAML::Value << step.position.wrist;
				out << YAML::Key << "gripper_rotation";
				out << YAML::Value << step.position.gripper_rotation;
				out << YAML::Key << "gripper";
				out << YAML::Value << step.position.gripper;

				out << YAML::EndMap;
			}

			out << YAML::EndSeq;
		}
		out << YAML::EndMap;
	}
	out << YAML::EndSeq;

	util::write_string_to_file(filename.c_str(), out.c_str());
}

void DeserializeRoboticArmAnimations(std::string filename) {
	if (util::read_string_from_file(filename.c_str()).second == 0) return;

	auto& roboticarm = registry.store<RoboticArm>();
	YAML::Node scene = YAML::LoadFile(filename);

	for (int i = 0; i < scene.size(); i++) {
		Animation animation;

		auto name = scene[i]["name"].as<std::string>();
		auto id = scene[i]["id"].as<UTIL_UUID>();
		auto loop = scene[i]["loop"].as<bool>();
		auto delay = scene[i]["delay"].as<float>();

		animation.delay = delay;
		animation.loop = loop;

		for (int ii = 0; ii < scene[i]["positions"].size(); ii++) {
			AnimationStep step;
			
			auto progress_len = scene[i]["positions"][ii]["progress_len"].as<float>();
			auto pow_t = scene[i]["positions"][ii]["pow_t"].as<float>();
			auto base = scene[i]["positions"][ii]["base"].as<float>();
			auto lower_elbow = scene[i]["positions"][ii]["lower_elbow"].as<float>();
			auto upper_elbow = scene[i]["positions"][ii]["upper_elbow"].as<float>();
			auto wrist = scene[i]["positions"][ii]["wrist"].as<float>();
			auto gripper_rotation = scene[i]["positions"][ii]["gripper_rotation"].as<float>();
			auto gripper = scene[i]["positions"][ii]["gripper"].as<float>();

			step.position = { base, lower_elbow, upper_elbow, wrist, gripper_rotation, gripper };
			step.pow_t = pow_t;
			step.progress_len = progress_len;

			animation.animation_step_array.push_back(step);
		}

		roboticarm.animation_registry[id] = { name, animation };
	}
}
