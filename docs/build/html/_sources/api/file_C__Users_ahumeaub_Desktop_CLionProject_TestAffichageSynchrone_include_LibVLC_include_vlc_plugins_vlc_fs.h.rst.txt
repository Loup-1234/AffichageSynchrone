
.. _file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_fs.h:

File vlc_fs.h
=============

|exhale_lsh| :ref:`Parent directory <dir_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS


.. contents:: Contents
   :local:
   :backlinks: none

Definition (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_fs.h``)
--------------------------------------------------------------------------------------------------------------------------


.. toctree::
   :maxdepth: 1

   program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_fs.h.rst



Detailed Description
--------------------

The functions in this file help with using low-level Unix-style file descriptors, BSD sockets and directories. In general, they retain the prototype and most semantics from their respective standard equivalents. However, there are a few differences:

- On Windows, file path arguments are expected in UTF-8 format. They are converted to UTF-16 internally, thus enabling access to paths outside of the local Windows ANSI code page.

- On POSIX systems, file descriptors are created with the close-on-exec flag set (atomically where possible), so that they do not leak to child process after fork-and-exec.

- vlc_scandir(), inspired by GNU scandir(), passes file names rather than dirent structure pointers to its callbacks.

- :ref:`exhale_function_group__sockets_1ga62d662e39d845037342ce07577113be5` takes an extra boolean for nonblocking mode (compare with the flags parameter in POSIX.next accept4()).

- Writing functions do not emit a SIGPIPE signal in case of broken pipe. 






Includes
--------


- ``dirent.h``

- ``sys/types.h``






Functions
---------


- :ref:`exhale_function_group__fd_1ga323c8f656dd7f8612a1c7a1eb74d0187`

- :ref:`exhale_function_group__fd_1ga6f00c8143a69db1eb119b8c174acb187`

- :ref:`exhale_function_group__file_1gad8675192624476fc15f28f89abd6a9bc`

- :ref:`exhale_function_group__dir_1ga89ff35e99ba8337809aa79602038de3e`

- :ref:`exhale_function_group__dir_1ga9eaea3781c8d9f1413af5530423d573e`

- :ref:`exhale_function_group__file_1gad1c02c4ebf7413a21cb90986007c0bad`

- :ref:`exhale_function_group__fd_1ga4dedb30ee2c959abc992de15b4d81607`

- :ref:`exhale_function_group__dir_1ga9e282d63fa02a73ea1ca9b2c688a2489`

- :ref:`exhale_function_group__fd_1ga49ed4726fbea4068224964070b768001`

- :ref:`exhale_function_group__fd_1gaff8546bcd073ab5f14cfcfea33ae970a`

- :ref:`exhale_function_group__fd_1gaefea762c641eadc895984c81b6938c6d`

- :ref:`exhale_function_group__dir_1ga41c608bd6a568eca71a508cf544cdb3f`

- :ref:`exhale_function_group__fd_1ga17e06dd3b2a0f8ffa4bdc1fa14e0dc6f`

- :ref:`exhale_function_group__dir_1ga63802dea65b9a23e11bbf08ad6ffd863`

- :ref:`exhale_function_group__file_1gafcf3430aa9165d0006aed45001b79698`

- :ref:`exhale_function_group__dir_1ga951a66572dda976ad620456417782e6a`

- :ref:`exhale_function_group__file_1ga30cd710c055ba3e4018e186fe1e2c607`

- :ref:`exhale_function_group__file_1gaa11d5cec32eccc0aa3b5be6c9af158a1`

- :ref:`exhale_function_group__fd_1gafb376789d9d716a4bbef5f8a19638d71`

- :ref:`exhale_function_group__fd_1ga6771710089889fa64d928b35ebb2a70c`

