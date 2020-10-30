#ifndef BUILD_SELF_HPP
#define BUILD_SELF_HPP

#include <vector>
#include <string>
#include <filesystem>

namespace BuildSelf {

struct Flags {

	bool run_after_compilation = false;
	bool release = false;

	static Flags parse(int argc, char** argv) noexcept;
};

struct Build {
	enum class Target {
		Header_Only,
		Exe
	};

	enum class Cli {
		Cl = 0,
		Gcc,
		Count
	};

	enum class Std_Ver {
		Cpp17 = 0,
		Count
	};

	std::string name;

	Flags flags;

	Cli cli;
	Target target;
	Std_Ver std_ver;
	std::filesystem::path compiler;
	std::vector<std::filesystem::path> source_files;
	std::vector<std::filesystem::path> header_files;
	std::vector<std::filesystem::path> link_files;

	std::vector<std::string> defines;

	static Build get_default(Flags flags = {}) noexcept;

	void add_source(const std::filesystem::path& f) noexcept;
	void add_source_recursively(const std::filesystem::path& f) noexcept;
	void add_library(const std::filesystem::path& f) noexcept;
	void add_header(const std::filesystem::path& f) noexcept;
	void add_define(std::string str) noexcept;
};

struct Commands {
	std::vector<std::string> commands;
};

};

#endif
#ifdef BS_implementation
#include <stdio.h>
#include <string.h>

#define NS BuildSelf

std::string get_compile_flag(NS::Build::Cli cli, NS::Build::Std_Ver std_ver) noexcept;
std::string get_object_output_flag(NS::Build::Cli cli, std::string_view str) noexcept;
std::string get_compile_flag(NS::Build::Cli cli) noexcept;
std::string get_include_flag(NS::Build::Cli cli, std::string_view str) noexcept;
std::string get_link_flag(NS::Build::Cli cli, std::string_view str) noexcept;
std::string get_define_flag(NS::Build::Cli cli, std::string_view str) noexcept;
std::string get_optimisation_flag(NS::Build::Cli cli) noexcept;

// FILE IO
void dump_to_file(std::string_view str, const std::filesystem::path& p) noexcept;
std::string read_file(const std::filesystem::path& p) noexcept;

NS::Flags NS::Flags::parse(int argc, char** argv) noexcept {
	NS::Flags flags;

	for (int i = 0; i < argc; ++i) {
		auto it = argv[i];

		if (strcmp(it, "--run") == 0) {
			flags.run_after_compilation = true;
		}
		if (strcmp(it, "--release") == 0) {
			flags.release = true;
		}
	}

	return flags;
}

// ===================== BUILD
NS::Build NS::Build::get_default(::NS::Flags flags) noexcept {
	NS::Build build;
	build.cli = Cli::Gcc;
	build.target = Target::Exe;
	build.std_ver = Std_Ver::Cpp17;
	build.compiler = "clang++";
	build.flags = flags;
	return build;
}
void NS::Build::add_library(const std::filesystem::path& f) noexcept {
	link_files.push_back(f);
}
void NS::Build::add_source(const std::filesystem::path& f) noexcept {
	source_files.push_back(f);
}
void NS::Build::add_source_recursively(const std::filesystem::path& f) noexcept {
	for (auto& x : std::filesystem::recursive_directory_iterator(f)) {
		if (!std::filesystem::is_regular_file(x)) continue;
		if (x.path().extension() != ".c" && x.path().extension() != ".cpp") continue;

		add_source(x);
	}
}

void NS::Build::add_header(const std::filesystem::path& f) noexcept {
	header_files.push_back(f);
}
void NS::Build::add_define(std::string str) noexcept {
	defines.push_back(std::move(str));
}

// ===================== BUILD

void concat(
	const std::filesystem::path& dst, const std::vector<std::filesystem::path>& src
) noexcept {
	std::string to_dump = read_file(dst);
	for (auto& x : src) to_dump += read_file(x);
	dump_to_file(to_dump, dst);
}
void append(
	const std::filesystem::path& dst, std::string_view src
) noexcept {
	std::string to_dump = read_file(dst);
	to_dump += src;
	dump_to_file(to_dump, dst);
}

std::string compile_script_header_only(const NS::Build& b) noexcept {
	std::string script;

	for (auto& x : b.header_files) script += read_file(x) + "\n";
	script += "#ifdef " + b.name + "_implementation\n";
	for (auto& x : b.source_files) {
		script += read_file(x) + "\n";
	}
	script += "#endif\n";
	return script;
}

NS::Commands compile_command_exe(const NS::Build& b) noexcept {
	NS::Commands commands;

	std::string command;

	std::vector<std::filesystem::path> object_files;
	// Compile to -o
	for (auto x : b.source_files) {
		auto c = x;
		auto o = "temp" / x.filename().replace_extension(".o");
		command = b.compiler.generic_string();
		command += " " + get_compile_flag(b.cli);
		if (b.flags.release) command += " " + get_optimisation_flag(b.cli);
		command += " " + get_object_output_flag(b.cli, o.generic_string());
		for (auto& d : b.defines) command += " " + get_define_flag(b.cli, d);
		command += " " + get_compile_flag(b.cli, b.std_ver);
		for (auto& x : b.header_files)
			command += " " + get_include_flag(b.cli, x.generic_string());
		command += " " + c.generic_string();

		commands.commands.push_back(command);
		object_files.push_back(o);
	}

	// Link
	command = b.compiler.generic_string() + " ";
	for (auto x : object_files) {
		command += x.generic_string() + " ";
	}
	for (auto& x : b.link_files) {
		command += get_link_flag(b.cli, x.generic_string()) + " ";
	}
	command += "-o " + b.name + ".exe ";
	commands.commands.push_back(command);

	return commands;
}

void execute(const NS::Commands& c) noexcept {
	std::filesystem::create_directory("temp");

	for (auto& x : c.commands) {
		printf("%s\n", x.c_str());
		system(x.c_str());
	}

	std::filesystem::remove_all("temp");
}

int main(int argc, char** argv) {
	extern NS::Build build(NS::Flags flags) noexcept;
	auto flags = NS::Flags::parse(argc - 1, argv + 1);

	auto b = build(flags);

	std::string to_dump;
	switch (b.target) {
	case NS::Build::Target::Header_Only : {
		to_dump = compile_script_header_only(b);
		dump_to_file(to_dump, b.name + ".hpp");
		break;
	}
	case NS::Build::Target::Exe : {
		auto c = compile_command_exe(b);
		execute(c);
		if (flags.run_after_compilation) {
			std::string run = b.name + ".exe";
			system(run.c_str());
		}
		break;
	}
	default:
		_ASSERT(false);
		break;
	}


	return 0;
}
#define main dummy_main

std::string get_compile_flag(NS::Build::Cli cli, NS::Build::Std_Ver std_ver) noexcept {
	const char* lookup[(size_t)NS::Build::Cli::Count][(size_t)NS::Build::Std_Ver::Count];
	lookup[(size_t)NS::Build::Cli::Gcc][(size_t)NS::Build::Std_Ver::Cpp17] = "-std=c++17";
	lookup[(size_t)NS::Build::Cli::Cl][(size_t)NS::Build::Std_Ver::Cpp17] = "/std:c++17";
	return lookup[(size_t)cli][(size_t)std_ver];
}

std::string get_optimisation_flag(NS::Build::Cli cli) noexcept {
	switch (cli) {
	case NS::Build::Cli::Gcc :
		return "-O3";
		break;
	case NS::Build::Cli::Cl :
		return "/O3";
		break;
	default:
		break;
	}
	return "";
}
std::string get_define_flag(NS::Build::Cli cli, std::string_view str) noexcept {
	std::string ret = "";
	switch (cli) {
	case NS::Build::Cli::Gcc :
		ret += "-D";
		ret += str.data();
		break;
	case NS::Build::Cli::Cl :
		ret += "/D";
		ret += str.data();
		break;
	default:
		break;
	}
	return ret;
}
std::string get_compile_flag(NS::Build::Cli cli) noexcept {
	switch (cli) {
	case NS::Build::Cli::Gcc :
		return "-c";
		break;
	case NS::Build::Cli::Cl :
		return "/c";
		break;
	default:
		return "";
		break;
	}
}
std::string get_link_flag(NS::Build::Cli cli, std::string_view str) noexcept {
	switch (cli) {
	case NS::Build::Cli::Gcc :
		return std::string("-l") + str.data();
		break;
	case NS::Build::Cli::Cl :
		return "";
		break;
	default:
		return "";
		break;
	}
}

std::string get_include_flag(NS::Build::Cli cli, std::string_view str) noexcept {
	switch (cli) {
	case NS::Build::Cli::Gcc :
		return std::string("-I") + str.data();
		break;
	case NS::Build::Cli::Cl :
		return std::string("/I ") + str.data();
		break;
	default:
		return "";
		break;
	}
}
std::string get_object_output_flag(NS::Build::Cli cli, std::string_view str) noexcept {
	switch (cli) {
	case NS::Build::Cli::Gcc :
		return std::string("-o ") + str.data();
		break;
	case NS::Build::Cli::Cl :
		return std::string("/Fo") + "\"" + str.data() + "\"";
		break;
	default:
		return "";
		break;
	}
}


void dump_to_file(std::string_view str, const std::filesystem::path& p) noexcept {
	FILE* f = fopen(p.generic_string().c_str(), "w");
	fprintf(f, "%s", str.data());
	fclose(f);
}

std::string read_file(const std::filesystem::path& p) noexcept {
	FILE *f = fopen(p.generic_string().c_str(), "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

	std::string string;
	string.resize(fsize);
	fread(string.data(), 1, fsize, f);
	fclose(f);

	return string;
}

#undef NS

#endif
