option(INSTALL_OPENRC "Install OpenRC script" ON)
option(INSTALL_RUNIT "Install Runit script" OFF)

install(FILES fcitx5-lotus-server@.service
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/system)

install(FILES 99-lotus.rules
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/udev/rules.d)

install(FILES module-fcitx5-lotus.conf
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modules-load.d
    RENAME fcitx5-lotus.conf)

install(FILES user-lotus.conf
    DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/sysusers.d
    RENAME lotus.conf)

if(INSTALL_OPENRC)
    install(PROGRAMS fcitx5-lotus.openrc
        DESTINATION /etc/init.d
        RENAME fcitx5-lotus)
endif(INSTALL_OPENRC)

if(INSTALL_RUNIT)
    install(PROGRAMS fcitx5-lotus.runit
        DESTINATION /etc/runit/sv/fcitx5-lotus
        RENAME run)
endif(INSTALL_RUNIT)
