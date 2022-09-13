#include "Serializer.h"
#include "ToYaml.h"
#include "ECS.h"
#include <LucyUtil/LucyUtil.h>
#include "RoboticArm.h"

static auto& registry = Registry::Instance();

void SerializeRoboticArmAnimations(std::string filename) {
	auto& roboticarm = registry.store<RoboticArm>();

	YAML::Emitter out;

	out << YAML::BeginMap;

	for (auto& pair: roboticarm.animation_registry) {
		out << YAML::Key << pair.first;
		out << YAML::Value;
		
		{
			out << YAML::BeginMap;

			out << YAML::Key << "name";
			out << YAML::Value << pair.second.name;
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
	}
	out << YAML::EndMap;

	util::write_string_to_file(filename.c_str(), out.c_str());
}

void DeserializeRoboticArmAnimations(std::string filename) {
	if (util::read_string_from_file(filename.c_str()).second == 0) return;

	YAML::Node scene = YAML::LoadFile(filename);
	
	// for (int i = 0; i != scene["Textures"].size(); i++) {
	// 	auto filename = scene["Textures"][i]["filename"].as<std::string>();
	// 	auto name = scene["Textures"][i]["name"].as<std::string>();
	// 	auto id = scene["Textures"][i]["id"].as<std::string>();

	// 	// spriteregistry.LoadTexture(name, filename, id);
	// }

	// for (int i = 0; i != scene["Sprites"].size(); i++) {
	// 	lucy::Component::Sprite sprite;

	// 	sprite.id = scene["Sprites"][i]["id"].as<std::string>();
	// 	sprite.raw_texture_id = scene["Sprites"][i]["raw_texture_id"].as<std::string>();
	// 	sprite.name = scene["Sprites"][i]["name"].as<std::string>();
	// 	sprite.uv0 = scene["Sprites"][i]["uv0"].as<glm::vec2>();
	// 	sprite.uv1 = scene["Sprites"][i]["uv1"].as<glm::vec2>();
	// 	sprite.idx = scene["Sprites"][i]["idx"].as<int>();

	// 	// spriteregistry.sprite_store[sprite.id] = sprite;
	// }
}
