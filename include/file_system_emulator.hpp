#ifndef __FILE_SYSTEM_EMULATOR_HPP__
#define __FILE_SYSTEM_EMULATOR_HPP__

#include <string_view>

#include "base.hpp"

/**
 * @class File_system_emulator
 *
 * Simulates a file system within a program. It supports operations such as creating
 * directories and files, creating hard and dynamic links, changing the current directory,
 * and removing files or directories along with their links. Additionally, it can copy, move,
 * delete entire directories (including their subdirectories), and print the structure of the
 * file system.
 */
class File_system_emulator
{
public:
  File_system_emulator() noexcept;

  ~File_system_emulator();

  /**
   * @brief Creates a new directory at the specified path if the intermediate path exists.
   *
   * @param path The full or relative path to the new directory.
   * @throws std::runtime_error If the path is not found or if a file with the same name already exists.
   */
  void
  make_dir(std::string_view path);

  /**
   * @brief Creates a new file at the specified path if the intermediate path exists.
   *
   * @param path The full or relative path to the new file.
   * @throws std::runtime_error If the path is not found or if a directory with the same name already exists.
   */
  void
  make_file(std::string_view path);

  /**
   * @brief Creates a new hard link for a file or directory if both source and destination paths exist.
   *
   * @param source The source path of the file/directory to link from.
   * @param dest The destination path for the new hard link.
   * @throws std::runtime_error If either the source or destination path is not found.
   */
  void
  make_hlink(std::string_view source, std::string_view dest);

  /**
   * @brief Creates a new dynamic link for a file or directory if both source and destination paths exist.
   *
   * @param source The source path of the file/directory to link from.
   * @param dest The destination path for the new dynamic link.
   * @throws std::runtime_error If either the source or destination path is not found.
   */
  void
  make_dlink(std::string_view source, std::string_view dest);

  /**
   * @brief Changes the current working directory to the specified path.
   *
   * @param path The path to the new current directory.
   * @throws std::runtime_error If the specified path is not found.
   */
  void
  change_dir(std::string_view path);

  /**
   * @brief Removes the specified directory and all its attached dynamic links.
   *
   * @param path The path to the directory to be removed.
   * @throws std::runtime_error If the path is not found, refers to the root or current directory,
   * is non-empty, or if the directory has attached hard links.
   */
  void
  remove_dir(std::string_view path);

  /**
   * @brief Removes the specified file and all its attached dynamic links.
   *
   * @param path The path to the file to be removed.
   * @throws std::runtime_error If the path is not found or if the file has attached hard links.
   */
  void
  remove_file(std::string_view path);

  /**
   * @brief Copies a directory along with its entire subtree to a new location.
   *
   * @param source The source path from which to copy.
   * @param dest The destination path where the copy will be placed.
   * @throws std::runtime_error If either the source or destination path is not found.
   */
  void
  copy(std::string_view source, std::string_view dest);

  /**
   * @brief Moves a directory along with its entire subtree to a new location.
   *
   * @param source The source path to move from.
   * @param dest The destination path where the source will be moved.
   * @throws std::runtime_error If either the source or destination path is not found, the source has attached
   * hard links, or if the source is the current or root directory.
   */
  void
  move(std::string_view source, std::string_view dest);

  /**
   * @brief Deletes an entire directory tree starting from the specified path.
   *
   * @param path The root path of the tree to delete.
   * @throws std::runtime_error If the path is not found, if any files within the subdirectories have attached
   * hard links, or if the path is the current or root directory.
   */
  void
  delete_tree(std::string_view path);

  /**
   * @brief Prints the structure of the file system to the standard output.
   */
  void
  print() const noexcept;

private:
  /**
   * @brief Converts a relative path to an absolute path based on a specified starting directory.
   *
   * @param path The relative or absolute path to convert.
   * @param dir The directory from which relative paths should start.
   * @return The absolute path as a string.
   */
  std::string
  m_to_absolute_path(std::string_view path, Directory* dir);

  /**
   * @brief Finds a node in the file system tree by a given path.
   *
   * @param path The path to search for.
   * @return A pointer to the found node, or nullptr if the node was not found.
   */
  Node*
  m_find_node_by_path(std::string_view path);

  /**
   * @brief Creates a new node in the file system tree at the specified path.
   *
   * @param path The path where the new node should be created.
   * @param name The name of the new node.
   * @param type The type of the new node (e.g., file or directory).
   * @throws std::runtime_error If the specified path is not found or valid.
   * @return A pointer to the created node.
   */
  Node*
  m_make_node(std::string_view path, std::string_view name, NODE_TYPE type);

  /**
   * @brief Creates a new link (hard or dynamic) and connects it to a source node.
   *
   * @param source The path to the file/directory to which the link will be attached.
   * @param dest The destination path where the new link will be placed.
   * @param name The name of the new link.
   * @param type The type of the link (hard or dynamic).
   * @throws std::runtime_error If the source or destination path is not found or is invalid.
   */
  void
  m_make_link(std::string_view source, std::string_view dest, std::string_view name, NODE_TYPE type);

  /**
   * @brief Removes a node from the file system tree.
   *
   * @param node The node to be removed.
   * @throws std::runtime_error If the node cannot be removed due to existing hard links.
   */
  void
  m_remove_node(Node* node);

  /**
   * @brief Recursively copies a node (and its subtree) to a new location.
   *
   * @param source The node to copy from.
   * @param destination The directory where the copied subtree will be placed.
   */
  void
  m_copy(Node* source, Directory* destination);

  /**
   * @brief Checks for the presence of hard links attached to a node, recursively examining sub-nodes.
   *
   * @param node The node to check.
   * @return True if hard links are found, otherwise False.
   */
  bool
  m_check_on_hlinks(Node* node);

  /**
   * @brief Updates the paths of hard and dynamic links associated with a node.
   *
   * @param node The node whose links need to be updated.
   */
  void
  m_update_links(Node* node);

  /**
   * @brief Recursively prints a node and its children to the standard output, with indentation representing depth.
   *
   * @param node The node to start printing from.
   * @param depth The current depth in the tree, used to determine indentation.
   */
  void
  m_print(Node* node, std::size_t depth) const noexcept;

private:
  Directory* m_root;         ///> root node of a tree, contains C: drive as it's child.
  Directory* m_curr_catalog; ///> Pointer to current directory in the tree.
};

#endif