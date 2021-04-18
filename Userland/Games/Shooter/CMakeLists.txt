set(SOURCES
    Game.cpp
    RendererView.cpp
    MapView.cpp
    main.cpp
)

serenity_app(Shooter ICON app-cube)
target_link_libraries(Shooter LibGUI)
