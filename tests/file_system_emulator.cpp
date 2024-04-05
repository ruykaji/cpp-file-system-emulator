#include <gtest/gtest.h>

#include "file_system_emulator.hpp"

TEST(File_system_emulator, Make_dir_no_throw_absolute_path)
{
  File_system_emulator fse__;

  EXPECT_NO_THROW(fse__.make_dir("C:\\Dir1"));
  EXPECT_NO_THROW(fse__.make_dir("C:\\Dir1\\Dir2"));
  EXPECT_NO_THROW(fse__.make_dir("C:\\BDir1"));
  EXPECT_NO_THROW(fse__.make_dir("C:\\BDir1\\CDir1"));

  fse__.print();
};

TEST(File_system_emulator, Make_dir_throw_absolute_path)
{
  File_system_emulator fse__;

  EXPECT_THROW(fse__.make_dir("C:\\Dir1\\Dir2"), std::runtime_error);

  fse__.make_dir("C:\\Dir1");
  fse__.make_file("C:\\Dir1\\file1");

  EXPECT_THROW(fse__.make_dir("C:\\Dir1\\file1"), std::runtime_error);

  fse__.print();
};

TEST(File_system_emulator, Make_file_no_throw_absolute_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  EXPECT_NO_THROW(fse__.make_file("C:\\Dir1\\file1.txt"));
  EXPECT_NO_THROW(fse__.make_file("C:\\Dir1\\file2.txt"));

  fse__.make_dir("C:\\BDir1");
  EXPECT_NO_THROW(fse__.make_file("C:\\BDir1\\file3.xt"));
  EXPECT_NO_THROW(fse__.make_file("C:\\BDir1\\file4.xt"));

  fse__.print();
};

TEST(File_system_emulator, Make_file_throw_absolute_path)
{
  File_system_emulator fse__;

  EXPECT_THROW(fse__.make_file("C:\\Dir1\\file1.txt"), std::runtime_error);

  fse__.make_dir("C:\\Dir1");
  fse__.make_dir("C:\\Dir1\\file1");

  EXPECT_THROW(fse__.make_file("C:\\Dir1\\file1"), std::runtime_error);

  fse__.print();
};

TEST(File_system_emulator, Make_dir_no_throw_relative_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.change_dir("C:\\Dir1");
  EXPECT_NO_THROW(fse__.make_dir("Dir2"));
  EXPECT_NO_THROW(fse__.make_dir("Dir2\\Dir3"));
  fse__.change_dir("C:");
  EXPECT_NO_THROW(fse__.make_dir("BDir1"));
  fse__.change_dir("C:\\BDir1");
  EXPECT_NO_THROW(fse__.make_dir("CDir1"));

  fse__.print();
};

TEST(File_system_emulator, Make_dir_throw_relative_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.change_dir("C:\\Dir1");
  EXPECT_THROW(fse__.make_dir("Dir2\\Dir3"), std::runtime_error);

  fse__.make_file("file1");
  EXPECT_THROW(fse__.make_dir("file1"), std::runtime_error);

  fse__.print();
};

TEST(File_system_emulator, Make_file_no_throw_relative_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.change_dir("C:\\Dir1");
  EXPECT_NO_THROW(fse__.make_file("file1"));
  EXPECT_NO_THROW(fse__.make_dir("file2"));
  fse__.change_dir("C:");
  EXPECT_NO_THROW(fse__.make_dir("BDir1"));
  fse__.change_dir("C:\\BDir1");
  EXPECT_NO_THROW(fse__.make_file("file3"));
  EXPECT_NO_THROW(fse__.make_file("file4"));

  fse__.print();
};

TEST(File_system_emulator, Make_file_throw_relative_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.change_dir("C:\\Dir1");
  fse__.make_dir("file1");
  EXPECT_THROW(fse__.make_file("Dir2\\file1"), std::runtime_error);
  EXPECT_THROW(fse__.make_file("file1"), std::runtime_error);

  fse__.print();
};

TEST(File_system_emulator, Make_link_no_throw_absolute_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.make_file("C:\\Dir1\\file1.txt");
  fse__.make_dir("C:\\Dir2");
  EXPECT_NO_THROW(fse__.make_hlink("C:\\Dir1\\file1.txt", "C:\\Dir2"));
  EXPECT_NO_THROW(fse__.make_hlink("C:\\Dir1", "C:\\Dir2"));

  EXPECT_NO_THROW(fse__.make_dlink("C:\\Dir1\\file1.txt", "C:\\Dir2"));
  EXPECT_NO_THROW(fse__.make_dlink("C:\\Dir1", "C:\\Dir2"));

  fse__.print();
};

TEST(File_system_emulator, Make_link_throw_absolute_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.make_file("C:\\Dir1\\file1.txt");
  fse__.make_dir("C:\\Dir2");

  EXPECT_THROW(fse__.make_hlink("C:\\Dir1\\file2.txt", "C:\\Dir2"), std::runtime_error);
  EXPECT_THROW(fse__.make_hlink("C:\\Dir1\\file1.txt", "C:\\Dir3"), std::runtime_error);

  EXPECT_THROW(fse__.make_dlink("C:\\Dir1\\file2.txt", "C:\\Dir2"), std::runtime_error);
  EXPECT_THROW(fse__.make_dlink("C:\\Dir1\\file1.txt", "C:\\Dir3"), std::runtime_error);

  fse__.print();
};

TEST(File_system_emulator, Make_link_no_throw_relative_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.make_file("C:\\Dir1\\file1.txt");
  fse__.make_dir("C:\\Dir2");
  fse__.change_dir("C:\\Dir1");
  EXPECT_NO_THROW(fse__.make_hlink("file1.txt", "C:\\Dir2"));
  EXPECT_NO_THROW(fse__.make_dlink("file1.txt", "C:\\Dir2"));

  fse__.print();
};

TEST(File_system_emulator, Make_link_throw_relative_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.make_file("C:\\Dir1\\file1.txt");
  fse__.make_dir("C:\\Dir2");
  fse__.change_dir("C:\\Dir1");

  EXPECT_THROW(fse__.make_hlink("file2.txt", "C:\\Dir2"), std::runtime_error);
  EXPECT_THROW(fse__.make_hlink("file1.txt", "C:\\Dir3"), std::runtime_error);

  EXPECT_THROW(fse__.make_dlink("file2.txt", "C:\\Dir2"), std::runtime_error);
  EXPECT_THROW(fse__.make_dlink("file1.txt", "C:\\Dir3"), std::runtime_error);

  fse__.print();
};

TEST(File_system_emulator, Remove_dir_no_throw_absolute_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.make_dir("C:\\Dir1\\Dir2");
  fse__.make_dlink("C:\\Dir1\\Dir2", "C:\\Dir1");
  EXPECT_NO_THROW(fse__.remove_dir("C:\\Dir1\\Dir2"));

  fse__.print();
};

TEST(File_system_emulator, Remove_dir_throw_absolute_path)
{
  File_system_emulator fse__;

  EXPECT_THROW(fse__.remove_dir("C:"), std::runtime_error);

  fse__.make_dir("C:\\Dir1");
  fse__.make_dir("C:\\Dir1\\Dir2");
  fse__.make_hlink("C:\\Dir1\\Dir2", "C:\\Dir1");

  EXPECT_THROW(fse__.remove_dir("C:\\Dir1"), std::runtime_error);
  EXPECT_THROW(fse__.remove_dir("C:\\Dir1\\Dir2"), std::runtime_error);

  fse__.change_dir("C:\\Dir1\\Dir2");
  EXPECT_THROW(fse__.remove_dir("C:\\Dir2"), std::runtime_error);

  fse__.print();
};

TEST(File_system_emulator, Remove_dir_no_throw_relative_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.make_dir("C:\\Dir1\\Dir2");
  fse__.make_dlink("C:\\Dir1\\Dir2", "C:\\Dir1");
  fse__.change_dir("C:\\Dir1");
  EXPECT_NO_THROW(fse__.remove_dir("Dir2"));

  fse__.print();
};

TEST(File_system_emulator, Remove_file_no_throw_absolute_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.make_dir("C:\\Dir1\\Dir2");

  fse__.make_file("C:\\Dir1\\file1.txt");
  fse__.make_dlink("C:\\Dir1\\file1.txt", "C:\\Dir1\\Dir2");
  EXPECT_NO_THROW(fse__.remove_file("C:\\Dir1\\file1.txt"));

  fse__.make_file("C:\\Dir1\\file1.txt");
  fse__.make_dlink("C:\\Dir1\\file1.txt", "C:\\Dir1\\Dir2");
  fse__.make_hlink("C:\\Dir1\\file1.txt", "C:\\Dir1\\Dir2");
  EXPECT_NO_THROW(fse__.remove_file("C:\\Dir1\\Dir2\\hlink[C:\\Dir1\\file1.txt]"));
  EXPECT_NO_THROW(fse__.remove_file("C:\\Dir1\\Dir2\\dlink[C:\\Dir1\\file1.txt]"));

  fse__.print();
};

TEST(File_system_emulator, Remove_file_throw_absolute_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.make_dir("C:\\Dir1\\Dir2");

  fse__.make_file("C:\\Dir1\\file1.txt");
  fse__.make_hlink("C:\\Dir1\\file1.txt", "C:\\Dir1\\Dir2");

  EXPECT_THROW(fse__.remove_file("C:\\Dir3\\file1.txt"), std::runtime_error);
  EXPECT_THROW(fse__.remove_file("C:\\Dir1\\file1.txt"), std::runtime_error);

  fse__.print();
};

TEST(File_system_emulator, Remove_file_no_throw_relative_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.make_dir("C:\\Dir1\\Dir2");

  fse__.make_file("C:\\Dir1\\file1.txt");
  fse__.make_dlink("C:\\Dir1\\file1.txt", "C:\\Dir1\\Dir2");
  fse__.change_dir("C:\\Dir1");
  EXPECT_NO_THROW(fse__.remove_file("file1.txt"));

  fse__.make_file("file1.txt");
  fse__.make_dlink("file1.txt", "C:\\Dir1\\Dir2");
  fse__.make_hlink("file1.txt", "C:\\Dir1\\Dir2");

  fse__.change_dir("C:\\Dir1\\Dir2");
  EXPECT_NO_THROW(fse__.remove_file("hlink[C:\\Dir1\\file1.txt]"));
  EXPECT_NO_THROW(fse__.remove_file("dlink[C:\\Dir1\\file1.txt]"));

  fse__.print();
};

TEST(File_system_emulator, Copy_no_throw_absolute_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.make_dir("C:\\Dir1\\Dir2");
  fse__.make_file("C:\\Dir1\\Dir2\\file1.txt");
  fse__.make_file("C:\\Dir1\\Dir2\\file2.txt");
  fse__.make_dlink("C:\\Dir1\\Dir2\\file2.txt", "C:\\Dir1\\Dir2");
  fse__.make_dir("C:\\BDir1");

  EXPECT_NO_THROW(fse__.copy("C:\\Dir1\\Dir2", "C:\\BDir1"));

  fse__.remove_file("C:\\Dir1\\Dir2\\file2.txt");

  fse__.print();
};

TEST(File_system_emulator, Copy_no_throw_relative_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.make_dir("C:\\Dir1\\Dir2");
  fse__.make_file("C:\\Dir1\\Dir2\\file1.txt");
  fse__.make_file("C:\\Dir1\\Dir2\\file2.txt");
  fse__.make_dlink("C:\\Dir1\\Dir2\\file2.txt", "C:\\Dir1\\Dir2");
  fse__.make_dir("C:\\BDir1");

  fse__.change_dir("C:\\Dir1");
  EXPECT_NO_THROW(fse__.copy("Dir2", "C:\\BDir1"));

  fse__.remove_file("Dir2\\file2.txt");

  fse__.print();
};

TEST(File_system_emulator, Move_no_throw_absolute_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.make_dir("C:\\Dir1\\Dir2");
  fse__.make_file("C:\\Dir1\\Dir2\\file1.txt");
  fse__.make_file("C:\\Dir1\\Dir2\\file2.txt");
  fse__.make_dlink("C:\\Dir1\\Dir2\\file2.txt", "C:\\Dir1\\Dir2");
  fse__.make_dlink("C:\\Dir1\\Dir2", "C:\\Dir1\\Dir2");
  fse__.make_dir("C:\\BDir1");

  fse__.change_dir("C:\\Dir1");
  EXPECT_NO_THROW(fse__.move("Dir2", "C:\\BDir1"));

  fse__.print();
};

TEST(File_system_emulator, Move_throw_absolute_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.make_dir("C:\\Dir1\\Dir2");
  fse__.make_file("C:\\Dir1\\Dir2\\file1.txt");
  fse__.make_file("C:\\Dir1\\Dir2\\file2.txt");
  fse__.make_hlink("C:\\Dir1\\Dir2\\file2.txt", "C:\\Dir1\\Dir2");
  fse__.make_dir("C:\\BDir1");

  EXPECT_THROW(fse__.move("C:\\Dir1\\Dir2", "C:\\BDir1"), std::runtime_error);

  fse__.print();
};

TEST(File_system_emulator, Delete_tree_no_throw_absolute_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.make_dir("C:\\Dir1\\Dir2");
  fse__.make_file("C:\\Dir1\\Dir2\\file1.txt");
  fse__.make_file("C:\\Dir1\\Dir2\\file2.txt");
  fse__.make_dir("C:\\BDir1");
  fse__.make_dlink("C:\\Dir1\\Dir2\\file2.txt", "C:\\BDir1");

  EXPECT_NO_THROW(fse__.delete_tree("C:\\Dir1"));

  fse__.print();
};

TEST(File_system_emulator, Delete_tree_no_throw_relative_path)
{
  File_system_emulator fse__;

  fse__.make_dir("C:\\Dir1");
  fse__.make_dir("C:\\Dir1\\Dir2");
  fse__.make_file("C:\\Dir1\\Dir2\\file1.txt");
  fse__.make_file("C:\\Dir1\\Dir2\\file2.txt");
  fse__.make_dir("C:\\BDir1");
  fse__.make_dlink("C:\\Dir1\\Dir2\\file2.txt", "C:\\BDir1");

  fse__.change_dir("C:\\Dir1");
  EXPECT_NO_THROW(fse__.delete_tree("C:\\Dir1\\Dir2"));

  fse__.print();
};

int
main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
