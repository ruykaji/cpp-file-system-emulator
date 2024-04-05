#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "file_system_emulator.hpp"

/**
 * Splits a command line string into its arguments based on spaces.
 *
 * @param line The command line string to be split.
 * @return A vector of string_views, each representing an argument from the command line.
 */
std::vector<std::string_view>
split_command_line(std::string_view line)
{
  std::vector<std::string_view> splitted__;
  std::size_t left_pos__ = 0;

  for(std::size_t curr_pos__ = 0, end__ = line.size(); curr_pos__ < end__; ++curr_pos__)
    {
      if(line.at(curr_pos__) == ' ')
        {
          splitted__.push_back(line.substr(left_pos__, curr_pos__ - left_pos__));
          left_pos__ = curr_pos__ + 1;
        }
    }

  splitted__.push_back(line.substr(left_pos__));

  return splitted__;
}

/**
 * @brief Converts a command string to lowercase.
 *
 * @param cmd The command string to convert.
 * @return A new string containing the lowercase version of the input command.
 */
std::string
get_command(std::string_view cmd)
{
  std::string cmd_name__{ cmd };
  std::transform(cmd_name__.begin(), cmd_name__.end(), cmd_name__.begin(), [](auto c) { return std::tolower(c); });
  return cmd_name__;
}

/**
 * @brief Validates a file or directory name based on specific rules.
 *
 * @param path The path or filename to validate.
 * @return True if the name meets the specified criteria, otherwise False.
 */
bool
is_valid_name(std::string_view path)
{
  // Checking if extension length bigger then have to be.
  std::size_t ext_idx__ = path.find_last_of('.');

  if(ext_idx__ != std::string::npos && (path.size() - ext_idx__ - 1) > 3)
    return false;

  std::string_view no_ext_path__ = path.substr(0, ext_idx__ - 1);

  // Checking if length of a name is bigger than have to be and if name include non-number and non-alpha characters.
  std::size_t idx__ = no_ext_path__.find_last_of('\\');
  std::string_view name__;

  if(idx__ == std::string::npos)
    {
      name__ = no_ext_path__.substr(0);

      if(no_ext_path__.size() > 8)
        return false;
    }
  else if(no_ext_path__.size() - idx__ > 8)
    return false;

  name__ = no_ext_path__.substr(idx__ + 1);

  for(auto c__ : name__)
    if(!std::isdigit(c__) && !std::isalpha(c__))
      return false;

  return true;
}

int
main(int argc, char const* argv[])
{
  if(argc == 1)
    throw std::runtime_error("ERROR: Expected bash file as input parameter but found nothing.");

  std::ifstream file__{ argv[1] };

  if(file__.good())
    {
      File_system_emulator fse__;

      std::string cmd_line__;

      try
        {
          while(std::getline(file__, cmd_line__))
            {
              if(cmd_line__.empty())
                continue;

              std::vector<std::string_view> splitted_line__ = split_command_line(cmd_line__);
              std::string cmd_name__ = get_command(splitted_line__.at(0));

              if(cmd_name__ == "md")
                {
                  if(splitted_line__.size() < 2)
                    throw std::runtime_error("ERROR: Not enough parameters for MD command.");

                  if(!is_valid_name(splitted_line__.at(1)))
                    throw std::runtime_error("ERROR: Invalid format of a directory name.");

                  fse__.make_dir(splitted_line__.at(1));
                }
              else if(cmd_name__ == "cd")
                {
                  if(splitted_line__.size() < 2)
                    throw std::runtime_error("ERROR: Not enough parameters for CD command.");

                  fse__.change_dir(splitted_line__.at(1));
                }
              else if(cmd_name__ == "rd")
                {
                  if(splitted_line__.size() < 2)
                    throw std::runtime_error("ERROR: Not enough parameters for RD command.");

                  fse__.remove_dir(splitted_line__.at(1));
                }
              else if(cmd_name__ == "deltree")
                {
                  if(splitted_line__.size() < 2)
                    throw std::runtime_error("ERROR: Not enough parameters for DELTREE command.");

                  fse__.delete_tree(splitted_line__.at(1));
                }
              else if(cmd_name__ == "mf")
                {
                  if(splitted_line__.size() < 2)
                    throw std::runtime_error("ERROR: Not enough parameters for MF command.");

                  if(!is_valid_name(splitted_line__.at(1)))
                    throw std::runtime_error("ERROR: Invalid format of a file name.");

                  fse__.make_file(splitted_line__.at(1));
                }
              else if(cmd_name__ == "mhl")
                {
                  if(splitted_line__.size() < 3)
                    throw std::runtime_error("ERROR: Not enough parameters for MHL command.");

                  fse__.make_hlink(splitted_line__.at(1), splitted_line__.at(2));
                }
              else if(cmd_name__ == "mdl")
                {
                  if(splitted_line__.size() < 3)
                    throw std::runtime_error("ERROR: Not enough parameters for MDL command.");

                  fse__.make_dlink(splitted_line__.at(1), splitted_line__.at(2));
                }
              else if(cmd_name__ == "del")
                {
                  if(splitted_line__.size() < 2)
                    throw std::runtime_error("ERROR: Not enough parameters for DEL command.");

                  fse__.remove_file(splitted_line__.at(1));
                }
              else if(cmd_name__ == "copy")
                {
                  if(splitted_line__.size() < 3)
                    throw std::runtime_error("ERROR: Not enough parameters for COPY command.");

                  fse__.copy(splitted_line__.at(1), splitted_line__.at(2));
                }
              else if(cmd_name__ == "move")
                {
                  if(splitted_line__.size() < 3)
                    throw std::runtime_error("ERROR: Not enough parameters for MOVE command.");

                  fse__.move(splitted_line__.at(1), splitted_line__.at(2));
                }
            }

          fse__.print();
        }
      catch(std::runtime_error exp)
        {
          fse__.print();
          std::cout << '\n' << exp.what() << '\n' << std::flush;
        }
    }

  return 0;
}
