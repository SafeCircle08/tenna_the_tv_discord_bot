#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <functional>
#include <unordered_map>
#include <dpp/dpp.h>
#include <random>
#include "config.h"

const std::vector<std::string> tennaPics = {
	"https://static.wikia.nocookie.net/villains/images/1/19/Tenna.png",
	"https://public-files.gumroad.com/vi4panwgfrn1c5kbbsqndd6gn7nx",
	"https://static.wikia.nocookie.net/deltarune/images/0/04/Cabbage_Dance.gif",
	"https://static.wikia.nocookie.net/deltarune/images/f/f9/Tenna_crashout_.gif",
	"https://static.wikia.nocookie.net/deltarune/images/4/49/Tenna_dance_2_.gif",
	"https://static.wikia.nocookie.net/deltarune/images/3/30/Tenna_overworld_listening.png",
	"https://static.wikia.nocookie.net/deltarune/images/1/1c/Tenna_overworld_whisper.png",
	"https://static.wikia.nocookie.net/deltarune/images/7/7b/Tenna_t_pose_.gif",
	"https://static.wikia.nocookie.net/deltarune/images/8/8e/Tenna_kick_.gif"
};

std::string generateRandomPhoto(const std::vector<std::string>& photosList)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(0, photosList.size() - 1);
	std::string randomPhoto = photosList[distr(gen)];
	return randomPhoto;
};

struct command {
	std::string name;
	std::string info;
};	

command createNewCommand
(
	dpp::cluster& bot, 
	std::unordered_map<std::string, std::function<void(const dpp::slashcommand_t& event, std::string&)>>& commands, 
	std::unordered_map<std::string, std::string>& commandsMessages,
	std::string cmdName, 
	std::string cmdInfo, 
	std::string replyMessage,
	std::function<void(const dpp::slashcommand_t&, std::string&)> commandFunc
)
{
	command cmd;
	cmd.name = cmdName;
	cmd.info = cmdInfo;

	bot.guild_command_create(
		dpp::slashcommand(cmdName, cmdInfo, bot.me.id),
		SERVER_ID
	);
	commands[cmdName] = commandFunc;
	commandsMessages[cmdName] = replyMessage;
	return cmd;
}

int main()
{
	std::unordered_map<std::string, std::function<void(const dpp::slashcommand_t& event, std::string& replyMessage)>> commands;
	std::unordered_map<std::string, std::string> commandsMessages;
	dpp::cluster bot(BOT_TOKEN);
	
	std::cout << "BOT CREATO" << std::endl;
	bot.guild_bulk_command_delete(SERVER_ID);

	bot.on_log(dpp::utility::cout_logger());

	bot.on_ready([&bot, &commands, &commandsMessages](const dpp::ready_t& event) {
		if (dpp::run_once<struct register_commands>()) {
			createNewCommand(bot, commands, commandsMessages, "ping", "Replies with Pong", "PONG!",
				[](const dpp::slashcommand_t& event, std::string& msg) { event.reply(msg); }
			);
			createNewCommand(bot, commands, commandsMessages, "love_tv", "Only if you love TV!", "I LOVE TV!",
				[](const dpp::slashcommand_t& event, std::string& msg) { event.reply(msg); }
			);
			createNewCommand(bot, commands, commandsMessages, "show_me_a_photo_of_yours", "Will send you a photo of Mine", "HERE IT IS MY PHOTO!",
				[](const dpp::slashcommand_t& event, std::string& replyMessage) { event.reply(generateRandomPhoto(tennaPics)); }
			);
		}
	});
	
	bot.on_slashcommand([&commands, &commandsMessages](const dpp::slashcommand_t& event) {
		auto it = commands.find(event.command.get_command_name());
		if (it != commands.end())
		{
			std::string& msg = commandsMessages[event.command.get_command_name()];
			it->second(event, msg);
		}
		else { event.reply("Couldn't the commands you put..."); }
	});
	bot.start(dpp::st_wait);
	return 0;
}