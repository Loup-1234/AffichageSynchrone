
.. _file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_playlist.h:

File vlc_playlist.h
===================

|exhale_lsh| :ref:`Parent directory <dir_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins>` (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins``)

.. |exhale_lsh| unicode:: U+021B0 .. UPWARDS ARROW WITH TIP LEFTWARDS


.. contents:: Contents
   :local:
   :backlinks: none

Definition (``C:\Users\ahumeaub\Desktop\CLionProject\TestAffichageSynchrone\include\LibVLC\include\vlc\plugins\vlc_playlist.h``)
--------------------------------------------------------------------------------------------------------------------------------


.. toctree::
   :maxdepth: 1

   program_listing_file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_playlist.h.rst



Detailed Description
--------------------

VLC playlist control interface

The VLC playlist system has a tree structure. This allows advanced categorization, like for SAP streams (which are grouped by "sap groups").

The base structure for all playlist operations is the :ref:`exhale_struct_structplaylist__item__t`. This is essentially a node within the playlist tree. Each playlist item references an :ref:`exhale_struct_structinput__item__t` which contains the input stream info, such as location, name and meta-data.

A playlist item is uniquely identified by its input item: playlist_ItemGetByInput(). A single input item cannot be used by more than one playlist item at a time; if necessary, a copy of the input item can be made instead.

The same playlist tree is visible to all user interfaces. To arbitrate access, a lock is used, see :ref:`exhale_function_group__playlist_1ga6917fc5e0c11b56d41891d99e04f6b1f` and :ref:`exhale_function_group__playlist_1gabad285726e1723f2c8cfe86eb91c209a`.

Under the playlist root item node, the top-level items are the main media sources and include:

- the actual playlist,

- the media library,

- the service discovery root node, whose children are services discovery module instances.



So, here is an example: * Inputs array
*  - input 1 -> name = foo 1 uri = ...
*  - input 2 -> name = foo 2 uri = ...
*
* Playlist items tree
* - playlist (id 1)
*    - category 1 (id 2)
*      - foo 2 (id 6 - input 2)
* - media library (id 2)
*    - foo 1 (id 5 - input 1)
* 

Sometimes, an item creates subitems. This happens for the directory access for example. In that case, if the item is under the "playlist" top-level item and playlist is configured to be flat then the item will be deleted and replaced with new subitems. If the item is under another top-level item, it will be transformed to a node and removed from the list of all items without nodes.

For "standard" item addition, you can use playlist_Add(), playlist_AddExt() (more options) or playlist_AddInput() if you already created your input item. This will add the item at the root of "Playlist" or of "Media library" in each of the two trees.

You can create nodes with playlist_NodeCreate() and can create items from existing input items to be placed under any node with playlist_NodeAddInput().

To delete an item, use playlist_NodeDelete( p_item ).

The playlist defines the following event variables:



- "item-change": It will contain a pointer to the :ref:`exhale_struct_structinput__item__t` of a changed input item monitored by the playlist.

- "playlist-item-append": It will contain a pointer to a :ref:`exhale_struct_structplaylist__item__t`.

- "playlist-item-deleted": It will contain a pointer to the :ref:`exhale_struct_structplaylist__item__t` about to be deleted.

- "leaf-to-parent": It will contain the playlist_item_t->i_id of an item that is transformed into a node.



The playlist contains rate-variable which is propagated to current input if available also rate-slower/rate-faster is in use. 




Includes
--------


- ``vlc_aout.h`` (:ref:`file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_plugins_vlc_aout.h`)

- ``vlc_events.h`` (:ref:`file_C__Users_ahumeaub_Desktop_CLionProject_TestAffichageSynchrone_include_LibVLC_include_vlc_libvlc_events.h`)






Classes
-------


- :ref:`exhale_struct_structplaylist__export__t`

- :ref:`exhale_struct_structplaylist__item__t`

- :ref:`exhale_struct_structplaylist__t`


Enums
-----


- :ref:`exhale_enum_group__playlist_1gab78b94c0d630b7c860ad202a13accc7a`

- :ref:`exhale_enum_group__playlist_1gad218ad3be0999259616ce9744637541b`

- :ref:`exhale_enum_group__playlist_1ga251542ab37148ec8b0303212af58c0c7`

- :ref:`exhale_enum_group__playlist_1gabb90addd20d32fb1e0e4bd8295440963`

- :ref:`exhale_enum_group__playlist_1ga8e081f83afd54626ee47766bdca9d807`

- :ref:`exhale_enum_group__playlist_1gaccdaf2247d258fb3d8d6cb008a7d0cfa`


Functions
---------


- :ref:`exhale_function_group__playlist_1gabb1ae0b4b3319324b4e823c399e1a9fa`

- :ref:`exhale_function_group__playlist_1ga7059c27e04b0ed57be06c9db48f131f6`

- :ref:`exhale_function_group__playlist_1ga27b715e142e43e7c3f04f699800a5fd0`

- :ref:`exhale_function_group__playlist_1ga466cc9208a59da48b69cf986b4134753`

- :ref:`exhale_function_group__playlist_1ga55ad0a4cc525a747f8a30ecf11b7644a`

- :ref:`exhale_function_group__playlist_1ga08a4c79407d2cde206127095aa7baa13`

- :ref:`exhale_function_group__playlist_1gaf0713fcbe280659eee8e66e684094627`

- :ref:`exhale_function_group__playlist_1ga708811d1ce4e5a45becf836db9bb10ee`

- :ref:`exhale_function_group__playlist_1ga0ec68dab4c46bd4af83b325a6dbd1a3b`

- :ref:`exhale_function_group__playlist_1ga7761033ba5ce467dba581ad2be3cc6d4`

- :ref:`exhale_function_group__playlist_1ga49050c7bcb39afad57f3fc32d45531ef`

- :ref:`exhale_function_group__playlist_1ga0a9ec4e662a6295fb348fbefe50e0e7f`

- :ref:`exhale_function_group__playlist_1gafc55ae4417238ba09c1633344f2ffa9c`

- :ref:`exhale_function_group__playlist_1ga9f5db83a8609d551d08425479d25ce20`

- :ref:`exhale_function_group__playlist_1ga829a994af03ea1707ce65756a609c490`

- :ref:`exhale_function_group__playlist_1ga4f24708ba949a52479dbeca63fe6f3f4`

- :ref:`exhale_function_group__playlist_1ga3433d0da5875ba97b0e42d6fd5d947fc`

- :ref:`exhale_function_group__playlist_1ga2e19167efd4a5b523ecc21743e7bd455`

- :ref:`exhale_function_group__playlist_1ga2cb2591cf33c213b064947677794ec67`

- :ref:`exhale_function_group__playlist_1ga25f25d19330ccc8435addf947ec14dee`

- :ref:`exhale_function_group__playlist_1ga5a3790945312c977bb9e8fd8fc74b1e5`

- :ref:`exhale_function_group__playlist_1gaaa934aab9789bdb675cdfe62abf490fc`

- :ref:`exhale_function_group__playlist_1ga6917fc5e0c11b56d41891d99e04f6b1f`

- :ref:`exhale_function_group__playlist_1ga437d8e1aeee3d3fec68e8eeb17b3ad6b`

- :ref:`exhale_function_group__playlist_1ga7ca388638e9741feb541bd8de3c6c908`

- :ref:`exhale_function_group__playlist_1ga9833e0914d3d95429110b22119d40b45`

- :ref:`exhale_function_group__playlist_1gad3067b576b0172bd88dc33d414bc6553`

- :ref:`exhale_function_group__playlist_1ga713880c55ff709d13407f26182c061bd`

- :ref:`exhale_function_group__playlist_1ga28a81c7e6d35b3886573bcd958153570`

- :ref:`exhale_function_group__playlist_1ga1a919ca0d54beff883e652ea514f9f1d`

- :ref:`exhale_function_group__playlist_1gabcaa5bfa4dfc14732daee3bbd0546b45`

- :ref:`exhale_function_group__playlist_1gafcfd2f1e287c4ca7a3356a1f4875c88d`

- :ref:`exhale_function_group__playlist_1gae68b5f21bde9697bf08ff9b3d0353105`

- :ref:`exhale_function_group__playlist_1ga91ffcf04d843ab08f7a460296d37595d`

- :ref:`exhale_function_group__playlist_1ga525a3ab08c2184247430d38bbcc39509`

- :ref:`exhale_function_group__playlist_1gae1131db3d617f68e37a987bf72351fc4`

- :ref:`exhale_function_group__playlist_1ga106875967d70575933fad24bb4b8a23b`

- :ref:`exhale_function_group__playlist_1ga438cbb7b1891623b0d3b75734cc0387f`

- :ref:`exhale_function_group__playlist_1gabad285726e1723f2c8cfe86eb91c209a`

- :ref:`exhale_function_group__playlist_1ga83c6564af11fe1bea96a7b51dc2358de`

- :ref:`exhale_function_group__playlist_1ga4046f9016d5d52eda5fe2d4673d2ae48`

- :ref:`exhale_function_group__playlist_1ga69ac5048543e0d2c48816e0741c16b61`

- :ref:`exhale_function_group__playlist_1gaa0115f1f6a566c028ce60cb322c04607`


Defines
-------


- :ref:`exhale_define_group__playlist_1ga74df88561cf7ca81beda662038f1e9f0`

- :ref:`exhale_define_group__playlist_1gaac776a4a1f7fdc735378489b1fdfae70`

- :ref:`exhale_define_group__playlist_1gae0bb6cd83832cb1d666af4ff6e78bd0d`

- :ref:`exhale_define_group__playlist_1ga2e039397f7ae3fbce43a4f21b18706f1`

- :ref:`exhale_define_group__playlist_1gabdc94229b10f802d8f19a64390fcd4ad`

- :ref:`exhale_define_group__playlist_1ga3a9d70247eacde47d114b3f39e554ded`

- :ref:`exhale_define_group__playlist_1ga71160158c9a5e66db171bb740b069f7d`

- :ref:`exhale_define_group__playlist_1gaa486e9bf03b67ffe9fff9e2708f2953a`

- :ref:`exhale_define_group__playlist_1ga7c0c3967494df733b2913120f9e18707`

- :ref:`exhale_define_group__playlist_1ga10d83c89e9310a74a8930b1d0e9eb0e0`

- :ref:`exhale_define_group__playlist_1ga37861e03e4bdbbbc40d665a4fdb58149`

- :ref:`exhale_define_group__playlist_1ga5d0caaa7af322ccd8ed554138bdc9376`

- :ref:`exhale_define_group__playlist_1gacb7c7b1f339dcf966b248b88f7ef4326`

- :ref:`exhale_define_group__playlist_1ga8ace70a6963b1b7bc25a92d1203b9358`

- :ref:`exhale_define_group__playlist_1gaeaa262aa1340666fc2f3d8a0271cdf6b`


Typedefs
--------


- :ref:`exhale_typedef_group__playlist_1ga0d92af566c7d7239f64af785fd4f01b9`

