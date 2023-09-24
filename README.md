# cpp-transport-catalogue
Транспортный Справочник (аналог 2Гис, Яндекс. Карты, Google. Карты), использующий парсинг JSON файлов, хэширование данных, рендеринг векторной графики (SVG) и поиск маршрутов через граф двух вершин (сборка под CMAKE, сериализацей с использованием Protobuf). Приложение позволяет эффективно и быстро строить и визуализировать базу маршрутов общественного транспорта, а также находить по ней кратчайшие пути следования, предоставляя пользователю любую статистическую информацию (время ожидания на остановках, расстояние между остановками и маршрута, время, затраченное в пути и др.)  
# Требования
C++17 и выше, Protobuf 21.1
# Описание
Из файла или входного потока считывается запрос в формате JSON, который содержит: а) данные для заполнения базы транспортного каталога; б) в конце запроса – данные, которые нужно посчитать, если запрос на визуализацию этих данных, то сформировать графическую карту в формате SVG,  и вывести также в формате JSON. 

Иллюстрация.
![renderer_architecture_1650925858](https://github.com/Evgenicast/cpp-transport-catalogue/assets/107400788/00da528f-7797-447e-bf88-c446992ff3a1) 

Главный принцип архитектуры приложения строится на концепции независимых модулей, подключаемых по мере необходимости. Данные модули подключаются путем агрегации (через ссылку на транспортный каталог).
При запросе вывести графичиский визуализацию базы данных транспортного справочника пользователь получает примерно следующее изображение.
![1111123_1_1656279827](https://github.com/Evgenicast/cpp-transport-catalogue/assets/107400788/1b512143-833e-4381-9795-7e01d3b392a5)

Пример. 
*Ввод*
````
  {
      "base_requests": [
          {
              "is_roundtrip": true,
              "name": "297",
              "stops": [
                  "Biryulyovo Zapadnoye",
                  "Biryulyovo Tovarnaya",
                  "Universam",
                  "Biryulyovo Zapadnoye"
              ],
              "type": "Bus"
          },
          {
              "is_roundtrip": false,
              "name": "635",
              "stops": [
                  "Biryulyovo Tovarnaya",
                  "Universam",
                  "Prazhskaya"
              ],
              "type": "Bus"
          },
          {
              "latitude": 55.574371,
              "longitude": 37.6517,
              "name": "Biryulyovo Zapadnoye",
              "road_distances": {
                  "Biryulyovo Tovarnaya": 2600
              },
              "type": "Stop"
          },
          {
              "latitude": 55.587655,
              "longitude": 37.645687,
              "name": "Universam",
              "road_distances": {
                  "Biryulyovo Tovarnaya": 1380,
                  "Biryulyovo Zapadnoye": 2500,
                  "Prazhskaya": 4650
              },
              "type": "Stop"
          },
          {
              "latitude": 55.592028,
              "longitude": 37.653656,
              "name": "Biryulyovo Tovarnaya",
              "road_distances": {
                  "Universam": 890
              },
              "type": "Stop"
          },
          {
              "latitude": 55.611717,
              "longitude": 37.603938,
              "name": "Prazhskaya",
              "road_distances": {},
              "type": "Stop"
          }
      ],
      "render_settings": {
          "bus_label_font_size": 20,
          "bus_label_offset": [
              7,
              15
          ],
          "color_palette": [
              "green",
              [
                  255,
                  160,
                  0
              ],
              "red"
          ],
          "height": 200,
          "line_width": 14,
          "padding": 30,
          "stop_label_font_size": 20,
          "stop_label_offset": [
              7,
              -3
          ],
          "stop_radius": 5,
          "underlayer_color": [
              255,
              255,
              255,
              0.85
          ],
          "underlayer_width": 3,
          "width": 200
      },
      "routing_settings": {
          "bus_velocity": 40,
          "bus_wait_time": 6
      },
      "stat_requests": [
          {
              "id": 1,
              "name": "297",
              "type": "Bus"
          },
          {
              "id": 2,
              "name": "635",
              "type": "Bus"
          },
          {
              "id": 3,
              "name": "Universam",
              "type": "Stop"
          },
		  {
              "id": 4,
              "type": "Map"
          },
          {
              "from": "Biryulyovo Zapadnoye",
              "id": 5,
              "to": "Universam",
              "type": "Route"
          },
          {
              "from": "Biryulyovo Zapadnoye",
              "id": 6,
              "to": "Prazhskaya",
              "type": "Route"
          }
      ]
  }
````
*Вывод*
````
[
    {
        "curvature": 1.42963,
        "request_id": 1,
        "route_length": 5990,
        "stop_count": 4,
        "unique_stop_count": 3
    },
    {
        "curvature": 1.30156,
        "request_id": 2,
        "route_length": 11570,
        "stop_count": 5,
        "unique_stop_count": 3
    },
    {
        "buses": [
            "297",
            "635"
        ],
        "request_id": 3
    },
    {
        "map": "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n  <polyline points=\"164.492,135.162 170,85.4419 147.56,97.7557 164.492,135.162\" fill=\"none\" stroke=\"green\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\" />\n  <polyline points=\"170,85.4419 147.56,97.7557 30,30 147.56,97.7557 170,85.4419\" fill=\"none\" stroke=\"rgb(255,160,0)\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\" />\n  <text x=\"164.492\" y=\"135.162\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\">297</text>\n  <text x=\"164.492\" y=\"135.162\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\" fill=\"green\">297</text>\n  <text x=\"170\" y=\"85.4419\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\">635</text>\n  <text x=\"170\" y=\"85.4419\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\" fill=\"rgb(255,160,0)\">635</text>\n  <text x=\"30\" y=\"30\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\">635</text>\n  <text x=\"30\" y=\"30\" dx=\"7\" dy=\"15\" font-size=\"20\" font-family=\"Verdana\" font-weight=\"bold\" fill=\"rgb(255,160,0)\">635</text>\n  <circle cx=\"170\" cy=\"85.4419\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"164.492\" cy=\"135.162\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"30\" cy=\"30\" r=\"5\" fill=\"white\"/>\n  <circle cx=\"147.56\" cy=\"97.7557\" r=\"5\" fill=\"white\"/>\n  <text x=\"170\" y=\"85.4419\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\">Biryulyovo Tovarnaya</text>\n  <text x=\"170\" y=\"85.4419\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"black\">Biryulyovo Tovarnaya</text>\n  <text x=\"164.492\" y=\"135.162\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\">Biryulyovo Zapadnoye</text>\n  <text x=\"164.492\" y=\"135.162\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"black\">Biryulyovo Zapadnoye</text>\n  <text x=\"30\" y=\"30\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\">Prazhskaya</text>\n  <text x=\"30\" y=\"30\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"black\">Prazhskaya</text>\n  <text x=\"147.56\" y=\"97.7557\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\">Universam</text>\n  <text x=\"147.56\" y=\"97.7557\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"black\">Universam</text>\n</svg> ",
        "request_id": 4
    },
    {
        "items": [
            {
                "stop_name": "Biryulyovo Zapadnoye",
                "time": 6,
                "type": "Wait"
            },
            {
                "bus": "297",
                "span_count": 2,
                "time": 5.235,
                "type": "Bus"
            }
        ],
        "request_id": 5,
        "total_time": 11.235
    },
    {
        "items": [
            {
                "stop_name": "Biryulyovo Zapadnoye",
                "time": 6,
                "type": "Wait"
            },
            {
                "bus": "297",
                "span_count": 2,
                "time": 5.235,
                "type": "Bus"
            },
            {
                "stop_name": "Universam",
                "time": 6,
                "type": "Wait"
            },
            {
                "bus": "635",
                "span_count": 1,
                "time": 6.975,
                "type": "Bus"
            }
        ],
        "request_id": 6,
        "total_time": 24.21
    }
]
````
# Установка Protobuf
Для сборки проекта потребуется установить Google Protocol Buffers. Для этого нужно:

Скачать Protobuf можно с репозитория на GitHub. Выберите архив protobuf-cpp с исходным кодом последней версии и распакуйте его на своём компьютере. Исходный код содержит CMake-проект.
Создать папки build-debug и build-release для сборки двух конфигураций Protobuf. Если будет использоваться Visual Studio, будет достаточно одной папки build. А если CLion или QT Creator, IDE автоматически создаст папки при открытии файла CMakeLists.txt.
Создать папку, в которой разместим пакет Protobuf. Будем называть её /path/to/protobuf/package(далее следует указывать абсолютный путь, а не относительный).
````
cmake <путь к protobuf>/cmake -DCMAKE_BUILD_TYPE=Debug \
      -Dprotobuf_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=/path/to/protobuf/package
cmake --build .
````
где:

Dprotobuf_BUILD_TESTS=OFF — чтобы не тратить время на сборку тестов,
DCMAKE_INSTALL_PREFIX=/path/to/protobuf/package — чтобы сообщить, где нужно будет создать пакет Protobuf.
Для Visual Studio команды немного другие. Конфигурация указывается не при генерации, а при сборке:
````
cmake <путь к protobuf>/cmake ^
      -Dprotobuf_BUILD_TESTS=OFF ^
      -DCMAKE_INSTALL_PREFIX=/path/to/protobuf/package ^
      -Dprotobuf_MSVC_STATIC_RUNTIME=OFF
cmake --build . --config Debug
````
После сборки библиотеки нужно запустить команду: cmake --install .  Под Visual Studio нужно указать конфигурацию, поскольку она не задавалась во время генерации: cmake --install . --config Debug 
Далее нужно проделать те же шаги с конфигурацией Release.
Важно! При сборке проекта, нужно указать путь к protobuf: DCMAKE_PREFIX_PATH=/path/to/protobuf/package
