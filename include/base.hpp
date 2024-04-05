#ifndef __BASE_HPP__
#define __BASE_HPP__

#include <forward_list>
#include <memory>
#include <set>
#include <string>

/**
 * @enum Enumerates the types of nodes that can exist within the file system emulator.
 *
 * DIRECTORY: Represents a directory that can contain other nodes (directories or files).
 * FILE: Represents a file, which may contain data and can be linked to.
 * HLINK: Represents a hard link, which acts as another reference to a file or directory.
 * DLINK: Represents a dynamic (soft) link, which acts as a shortcut to another file or directory.
 */
enum class NODE_TYPE
{
  DIRECTORY = 0,
  FILE,
  HLINK,
  DLINK,
};

struct Directory;

/**
 *@brief Represents the base node within the File System Emulator (FSE) tree. This is an abstract
 * base class for all nodes (directories, files, and links) within the file system.
 */
struct Node
{
  Node(NODE_TYPE type) noexcept : m_parent(nullptr), m_type(type), m_name(){};

  virtual ~Node() = default;

  Directory* m_parent;
  NODE_TYPE m_type;
  std::string m_name;
};

/**
 * @brief Extends the basic Node structure to include support for hard and dynamic links.
 * This class serves as the base for nodes that can be linked to, such as files and directories.
 *
 * m_hlinks: A list of nodes that are hard-linked to this node.
 * m_dlinks: A list of nodes that are dynamic links to this node.
 */
struct Linked_node : Node
{
  Linked_node(NODE_TYPE type) noexcept : Node(type), m_hlinks(), m_dlinks(){};

  std::forward_list<Node*> m_hlinks;
  std::forward_list<Node*> m_dlinks;
};

/**
 * @brief Represents a directory within the file system. It extends Linked_node to include
 * the capability to have child nodes, making it possible to build a hierarchical
 * structure of files and directories.
 */
struct Directory : Linked_node
{
  Directory() noexcept : Linked_node(NODE_TYPE::DIRECTORY), m_childs(){};

  ~Directory() noexcept
  {
    for(auto child__ : m_childs)
      delete child__;
  }

  std::forward_list<Node*> m_childs;
};

/**
 * @brief Represents a file within the file system. It extends Linked_node but does not
 * introduce additional members beyond those inherited from Linked_node, as the basic
 * file does not need to hold child nodes like a directory.
 */
struct File : Linked_node
{
  File() noexcept : Linked_node(NODE_TYPE::FILE){};

  ~File() = default;
};

#endif