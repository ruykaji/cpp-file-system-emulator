#include <algorithm>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <vector>

#include "file_system_emulator.hpp"

static constexpr char DRIVE[3] = "C:";

/*
 * *****************************************************************
 * *                  Path utility functions                      *
 * *****************************************************************
 */

/**
 * @brief Determines if the provided path is an absolute path.
 *
 * @param path The path to evaluate.
 * @return True if the path is absolute, otherwise False.
 */
bool
is_absolute_path(std::string_view path)
{
  return path.starts_with(DRIVE);
}

/**
 * @brief Extracts the path to the parent directory from a given path.
 *
 * @param path The complete path from which to extract the parent directory's path.
 * @return A std::string_view representing the path to the parent directory. Returns an empty
 * std::string_view if the path does not contain a directory separator.
 */
static std::string_view
get_parent_path(std::string_view path)
{
  std::size_t idx__ = path.find_last_of('\\');

  if(idx__ != std::string::npos)
    return path.substr(0, idx__);
  return {};
}

/**
 * @brief Retrieves the basename (the file or directory name) from a given path.
 *
 * @param path The path from which to extract the basename.
 * @return A std::string_view of the basename. If the path ends with a separator, returns an
 * empty std::string_view.
 */
static std::string_view
get_path_basename(std::string_view path)
{
  std::size_t idx__ = path.find_last_of('\\');
  return path.substr(idx__ + 1);
}

/**
 * @brief Splits a given path into its constituent directory and file names.
 *
 * @param path The full path to split into segments.
 * @return A vector of std::string_view, each representing a segment of the path (directory or file names).
 */
static std::vector<std::string_view>
split_path(std::string_view path)
{
  std::vector<std::string_view> path_list__;

  std::size_t left_pos__ = 0;

  for(std::size_t curr_pos__ = 0, end__ = path.size(); curr_pos__ < end__; ++curr_pos__)
    {
      if(path.at(curr_pos__) == '\\')
        {
          path_list__.push_back(path.substr(left_pos__, curr_pos__ - left_pos__));
          left_pos__ = curr_pos__ + 1;
        }
      else if(path.at(curr_pos__) == '[')
        break;
    }

  path_list__.push_back(path.substr(left_pos__));

  return path_list__;
}

/**
 * @brief Extracts the name of the entity to which a hard or dynamic link points from the link's name.
 *
 * @param name The name of the link, including the target entity's name in square brackets.
 * @return A std::string_view of the target entity's name. If the name does not conform to the expected
 * format, the behavior is undefined.
 */
static std::string_view
get_link_basename(std::string_view name)
{
  std::size_t left__ = name.find_first_of('[');
  std::size_t right__ = name.find_first_of(']');
  return name.substr(left__ + 1, right__ - left__ - 1);
}

/*
 * *****************************************************************
 * *             File_system_emulator method definitions           *
 * *****************************************************************
 */

File_system_emulator::File_system_emulator() noexcept
{
  m_curr_catalog = new Directory();
  m_curr_catalog->m_name = DRIVE;

  m_root = new Directory();
  m_root->m_childs.push_front(m_curr_catalog);
};

File_system_emulator::~File_system_emulator()
{
  delete m_root;
}

void
File_system_emulator::make_dir(std::string_view path)
{
  std::string_view parent_path__ = get_parent_path(path);
  std::string_view node_name__ = get_path_basename(path);
  m_make_node(parent_path__, node_name__, NODE_TYPE::DIRECTORY);
}

void
File_system_emulator::make_file(std::string_view path)
{
  std::string_view parent_path__ = get_parent_path(path);
  std::string_view node_name__ = get_path_basename(path);
  m_make_node(parent_path__, node_name__, NODE_TYPE::FILE);
}

void
File_system_emulator::make_hlink(std::string_view source, std::string_view dest)
{
  // Make correct hlink destination path 'cause %dest doesn't contain name of a hlink.
  std::string full_path_to_source__ = m_to_absolute_path(source, m_curr_catalog);
  std::string link_name__ = "hlink[" + full_path_to_source__ + "]";
  m_make_link(source, dest, link_name__, NODE_TYPE::HLINK);
}

void
File_system_emulator::make_dlink(std::string_view source, std::string_view dest)
{
  // Make correct dlink destination path 'cause %dest doesn't contain name of a dlink.
  std::string full_path_to_source__ = m_to_absolute_path(source, m_curr_catalog);
  std::string link_name__ = "dlink[" + full_path_to_source__ + "]";
  m_make_link(source, dest, link_name__, NODE_TYPE::DLINK);
}

void
File_system_emulator::change_dir(std::string_view path)
{
  Node* node_ptr__ = m_find_node_by_path(path);

  if(!node_ptr__ || node_ptr__->m_type != NODE_TYPE::DIRECTORY)
    throw std::runtime_error("ERROR: Path not found.");

  m_curr_catalog = static_cast<Directory*>(node_ptr__);
}

void
File_system_emulator::remove_dir(std::string_view path)
{
  Node* node_ptr__ = m_find_node_by_path(path);

  if(!node_ptr__ || node_ptr__->m_type != NODE_TYPE::DIRECTORY)
    throw std::runtime_error("ERROR: Path is not found.");

  Directory* dir_ptr__ = static_cast<Directory*>(node_ptr__);

  if(dir_ptr__ == m_root->m_childs.front())
    throw std::runtime_error("ERROR: Can`t delete root directory.");

  if(dir_ptr__ == m_curr_catalog)
    throw std::runtime_error("ERROR: Can`t delete current directory.");

  if(!dir_ptr__->m_childs.empty())
    throw std::runtime_error("ERROR: Can`t delete non-empty directory");

  m_remove_node(node_ptr__);
}

void
File_system_emulator::remove_file(std::string_view path)
{
  Node* node_ptr__ = m_find_node_by_path(path);

  if(!node_ptr__ || node_ptr__->m_type == NODE_TYPE::DIRECTORY)
    throw std::runtime_error("ERROR: Path is not found.");

  m_remove_node(node_ptr__);
}

void
File_system_emulator::copy(std::string_view source, std::string_view dest)
{
  Node* source_stpr__ = m_find_node_by_path(source);

  if(!source_stpr__)
    throw std::runtime_error("ERROR: Path is not found.");

  Node* dest_ptr__ = m_find_node_by_path(dest);

  if(!dest_ptr__ || dest_ptr__->m_type != NODE_TYPE::DIRECTORY)
    throw std::runtime_error("ERROR: Path is not found.");

  m_copy(source_stpr__, static_cast<Directory*>(dest_ptr__));
}

void
File_system_emulator::move(std::string_view source, std::string_view dest)
{
  Node* source_ptr__ = m_find_node_by_path(source);

  if(!source_ptr__)
    throw std::runtime_error("ERROR: Path is not found.");

  Node* dest_ptr__ = m_find_node_by_path(dest);

  if(!dest_ptr__ || dest_ptr__->m_type != NODE_TYPE::DIRECTORY)
    throw std::runtime_error("ERROR: Path is not found.");

  if(dest_ptr__ == source_ptr__ || source_ptr__ == m_root->m_childs.front())
    return;

  if(source_ptr__->m_type == NODE_TYPE::DIRECTORY)
    {
      Directory* dir_ptr__ = static_cast<Directory*>(source_ptr__);

      // Traverse tree from current source directory to try find any entity that have attached hard link.
      if(m_check_on_hlinks(dir_ptr__))
        throw std::runtime_error("ERROR: Can't move source with attached hard link.");
    }
  else if(source_ptr__->m_type == NODE_TYPE::FILE)
    {
      File* file_ptr__ = static_cast<File*>(source_ptr__);

      if(!file_ptr__->m_hlinks.empty())
        throw std::runtime_error("ERROR: Can't move source with attached hard link.");
    }

  Directory* source_parent_dir_ptr__ = static_cast<Directory*>(source_ptr__->m_parent);
  source_parent_dir_ptr__->m_childs.remove(source_ptr__);

  Directory* dest_dir_ptr__ = static_cast<Directory*>(dest_ptr__);
  dest_dir_ptr__->m_childs.push_front(source_ptr__);

  source_ptr__->m_parent = dest_dir_ptr__;

  m_update_links(source_ptr__);
}

void
File_system_emulator::delete_tree(std::string_view path)
{
  Node* node_ptr__ = m_find_node_by_path(path);

  if(!node_ptr__ || node_ptr__->m_type != NODE_TYPE::DIRECTORY)
    throw std::runtime_error("ERROR: Path is not found.");

  Directory* target_dir_ptr__ = static_cast<Directory*>(node_ptr__);

  if(target_dir_ptr__ == m_root->m_childs.front())
    throw std::runtime_error("ERROR: Can`t delete root directory.");

  if(target_dir_ptr__ == m_curr_catalog)
    throw std::runtime_error("ERROR: Can`t delete current directory.");

  // Apply BFS to delete one by one each element from current tree.
  // Deletion continues until either current tree is empty either throw of an exception.
  std::queue<Node*> queue__;

  while(!target_dir_ptr__->m_childs.empty())
    {
      queue__.push(target_dir_ptr__);

      while(!queue__.empty())
        {
          Node* node_ptr__ = queue__.front();
          queue__.pop();

          if(node_ptr__->m_type != NODE_TYPE::DIRECTORY)
            m_remove_node(node_ptr__);
          else
            {
              Directory* dir_ptr__ = static_cast<Directory*>(node_ptr__);

              if(dir_ptr__->m_childs.empty())
                m_remove_node(dir_ptr__);
              else
                {
                  for(auto child__ : dir_ptr__->m_childs)
                    queue__.push(child__);
                }
            }
        }
    }

  Directory* parent__ = node_ptr__->m_parent;
  parent__->m_childs.remove(node_ptr__);

  delete node_ptr__;
};

void
File_system_emulator::print() const noexcept
{
  std::cout << '\n';
  m_print(m_root->m_childs.front(), 0);
  std::cout << '\n' << std::flush;
}

std::string
File_system_emulator::m_to_absolute_path(std::string_view path, Directory* dir)
{
  if(is_absolute_path(path))
    return std::string(path);

  std::string absolute_path__{ path };

  while(dir)
    {
      absolute_path__ = dir->m_name + '\\' + absolute_path__;
      dir = dir->m_parent;
    }

  return absolute_path__;
}

Node*
File_system_emulator::m_find_node_by_path(std::string_view path)
{
  // Can occur if relative path is something like "Dir" so there is no parent path.
  if(path.empty())
    return m_curr_catalog;

  // Choose start point of iteration over fse tree.
  Directory* curr__ = is_absolute_path(path) ? m_root : m_curr_catalog;
  // Get all node names from path for further search.
  std::vector<std::string_view> path_list__ = split_path(path);

  bool is_node_exists = false;

  for(auto entity_name__ : path_list__)
    {
      is_node_exists = false;

      for(auto child : curr__->m_childs)
        {
          if(child->m_name == entity_name__)
            {
              is_node_exists = true;

              if(child->m_type == NODE_TYPE::DIRECTORY)
                {
                  curr__ = static_cast<Directory*>(child);
                  break;
                }
              else
                return child;
            }
        }

      // If next subdirectory was not found then provided path doesn't exists.
      if(!is_node_exists)
        return nullptr;
    }

  return curr__;
}

Node*
File_system_emulator::m_make_node(std::string_view path, std::string_view name, NODE_TYPE type)
{
  Node* node_ptr__ = m_find_node_by_path(path);

  if(!node_ptr__ || node_ptr__->m_type != NODE_TYPE::DIRECTORY)
    throw std::runtime_error("ERROR: Path not found.");

  Directory* parent_dir__ = static_cast<Directory*>(node_ptr__);

  // Checking if there any entity with same name...
  for(auto child__ : parent_dir__->m_childs)
    {
      if(child__->m_name == name)
        {
          // If types and names are equal then this attempt to create the same entity with the same name,
          // then just create nothing...
          if(child__->m_type == type)
            return nullptr;

          if(child__->m_type == NODE_TYPE::FILE)
            throw std::runtime_error("ERROR: Can`t create a directory - File with the same name exists.");
          if(child__->m_type == NODE_TYPE::DIRECTORY)
            throw std::runtime_error("ERROR: Can`t create a file - Directory with the same name exists.");
        }
    }

  Node* new_node_ptr__;

  switch(type)
    {
    case NODE_TYPE::FILE: new_node_ptr__ = new File(); break;
    case NODE_TYPE::HLINK: new_node_ptr__ = new Node(type); break;
    case NODE_TYPE::DLINK: new_node_ptr__ = new Node(type); break;
    case NODE_TYPE::DIRECTORY: new_node_ptr__ = new Directory(); break;
    default: break;
    }

  new_node_ptr__->m_name = name;
  new_node_ptr__->m_parent = parent_dir__;

  parent_dir__->m_childs.push_front(new_node_ptr__);

  return new_node_ptr__;
}

void
File_system_emulator::m_make_link(std::string_view source, std::string_view dest, std::string_view name, NODE_TYPE type)
{
  Node* source_ptr__ = m_find_node_by_path(source);

  if(!source_ptr__)
    throw std::runtime_error("ERROR: Path is not found.");

  Node* link__ = m_make_node(dest, name, type);

  // If link with the same name no present by this path.
  if(link__)
    {
      Linked_node* linked_node__ = static_cast<Linked_node*>(source_ptr__);

      if(link__->m_type == NODE_TYPE::HLINK)
        linked_node__->m_hlinks.push_front(link__);
      else
        linked_node__->m_dlinks.push_front(link__);
    }
}

void
File_system_emulator::m_remove_node(Node* node)
{
  if(node->m_type == NODE_TYPE::FILE || node->m_type == NODE_TYPE::DIRECTORY)
    {
      Linked_node* linked_node_ptr__ = static_cast<Linked_node*>(node);

      if(!linked_node_ptr__->m_hlinks.empty())
        throw std::runtime_error("ERROR: Can`t delete entity with attached hard link.");

      // Delete all dynamic links that attached to this node.
      while(!linked_node_ptr__->m_dlinks.empty())
        {
          Node* child__ = linked_node_ptr__->m_dlinks.front();
          Directory* parent__ = child__->m_parent;

          parent__->m_childs.remove(child__);
          linked_node_ptr__->m_dlinks.pop_front();

          delete child__;
        }
    }

  Directory* parent__ = node->m_parent;
  parent__->m_childs.remove(node);

  delete node;
};

void
File_system_emulator::m_copy(Node* source, Directory* destination)
{
  switch(source->m_type)
    {
    case NODE_TYPE::FILE:
      {
        File* file_ptr__ = new File();
        file_ptr__->m_name = source->m_name;
        file_ptr__->m_parent = destination;

        destination->m_childs.push_front(file_ptr__);
        break;
      }
    case NODE_TYPE::HLINK:
      {
        Node* hlink_ptr__ = new Node(NODE_TYPE::HLINK);
        hlink_ptr__->m_name = source->m_name;
        hlink_ptr__->m_parent = destination;

        std::string_view linked_node_path__ = get_link_basename(hlink_ptr__->m_name);
        Linked_node* linked_node_ptr__ = static_cast<Linked_node*>(m_find_node_by_path(linked_node_path__));

        linked_node_ptr__->m_hlinks.push_front(hlink_ptr__);
        destination->m_childs.push_front(hlink_ptr__);
        break;
      }
    case NODE_TYPE::DLINK:
      {
        Node* dlink_ptr__ = new Node(NODE_TYPE::DLINK);
        dlink_ptr__->m_name = source->m_name;
        dlink_ptr__->m_parent = destination;

        std::string_view linked_node_path__ = get_link_basename(dlink_ptr__->m_name);
        Linked_node* linked_node_ptr__ = static_cast<Linked_node*>(m_find_node_by_path(linked_node_path__));

        linked_node_ptr__->m_dlinks.push_front(dlink_ptr__);
        destination->m_childs.push_front(dlink_ptr__);
        break;
      }
    case NODE_TYPE::DIRECTORY:
      {
        Directory* dir_ptr__ = new Directory();
        dir_ptr__->m_name = source->m_name;
        dir_ptr__->m_parent = destination;

        Directory* source_as_dir__ = static_cast<Directory*>(source);

        for(auto child : source_as_dir__->m_childs)
          m_copy(child, dir_ptr__);

        destination->m_childs.push_front(dir_ptr__);
        break;
      }
    default: break;
    }
}

bool
File_system_emulator::m_check_on_hlinks(Node* node)
{
  if(node->m_type == NODE_TYPE::FILE)
    {
      Linked_node* linked_node_ptr__ = static_cast<Linked_node*>(node);

      if(!linked_node_ptr__->m_hlinks.empty())
        return true;
    }
  else if(node->m_type == NODE_TYPE::DIRECTORY)
    {
      Directory* dir_ptr__ = static_cast<Directory*>(node);

      if(!dir_ptr__->m_hlinks.empty())
        return true;

      for(auto child__ : dir_ptr__->m_childs)
        if(m_check_on_hlinks(child__))
          return true;
    }

  return false;
}

void
File_system_emulator::m_update_links(Node* node)
{
  if(node->m_type == NODE_TYPE::DIRECTORY || node->m_type == NODE_TYPE::FILE)
    {
      Linked_node* linked_node__ = static_cast<Linked_node*>(node);

      if(!linked_node__->m_dlinks.empty())
        {
          std::string updated_path__ = m_to_absolute_path(linked_node__->m_name, linked_node__->m_parent);

          for(auto dlink__ : linked_node__->m_dlinks)
            dlink__->m_name = "dlink[" + updated_path__ + "]";
        }
    }

  if(node->m_type == NODE_TYPE::DIRECTORY)
    {
      Directory* dir_ptr__ = static_cast<Directory*>(node);

      for(auto child__ : dir_ptr__->m_childs)
        m_update_links(child__);
    }
}

void
File_system_emulator::m_print(Node* node, std::size_t depth) const noexcept
{
  struct
  {
    bool
    operator()(Node* lhs, Node* rhs)
    {
      return lhs->m_name < rhs->m_name;
    }
  } comp__;

  if(node->m_type == NODE_TYPE::DIRECTORY)
    {
      Directory* dir_ptr__ = static_cast<Directory*>(node);
      dir_ptr__->m_childs.sort(comp__);

      for(std::size_t i = 0; i < depth; ++i)
        std::cout << ((i == depth - 1) ? "|_" : "| ");

      std::cout << node->m_name << '\n';

      for(auto child : dir_ptr__->m_childs)
        m_print(child, depth + 1);
    }
  else
    {
      for(std::size_t i = 0; i < depth; ++i)
        std::cout << ((i == depth - 1) ? "|_" : "| ");

      std::cout << node->m_name << '\n';
    }
}