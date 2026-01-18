# Snake (C++23 + SFML)

Projekt to klasyczna gra Snake napisana w C++23 z u¿yciem SFML (render + eventy). Logika gry jest oparta o siatkê pól (Grid), a warstwa graficzna tylko mapuje pola na piksele.

## Opis gry
Gracz steruje wê¿em poruszaj¹cym siê po planszy. W¹¿ zjada jedzenie pojawiaj¹ce siê losowo na wolnych polach. Po zjedzeniu jedzenia w¹¿ roœnie o 1 segment. Gra koñczy siê, gdy w¹¿ uderzy w œcianê lub we w³asne cia³o.

Na starcie gracz podaje nick w oknie gry. Wynik i tabela najlepszych wyników (Top 3) s¹ zapisywane w pliku tekstowym.

## Sterowanie
- Strza³ki / WASD: zmiana kierunku
- P: pauza
- R: restart
- Esc: wyjœcie

## Architektura i podzia³ odpowiedzialnoœci
Projekt jest podzielony na prost¹ logikê i warstwê SFML.

## Elementy C++ i STL wykorzystane w projekcie
- kontenery: `std::deque`, `std::vector`
- algorytmy i podejœcie “czytelny STL”: wyszukiwanie i sortowanie w highscore, sprawdzenia kolizji
- wyj¹tki (`std::runtime_error`, `std::invalid_argument`) do obs³ugi b³êdów konfiguracji i zasobów
- `std::filesystem` do pracy z katalogiem `data` i œcie¿kami
- `std::fstream` do zapisu/odczytu `highscore.txt`

## Dane gry (katalog `data`)
- `data/config.txt` - rozmiar planszy, wielkoœæ kafla, czas ticka (ms)
- `data/highscore.txt` - Top 3 wyników w formacie: `NICK WYNIK`
- `data/JetBrainsMono-Regular.ttf` - font do renderowania tekstu



[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/9YMgFLNa)
# Szablon projektu C++

- Pliki Ÿród³owe (`.cpp`) dodawaj do folderu `src`.
- Pliki nag³ówkowe (`.hpp`) dodawaj do folderu `include`.
- Je¿eli do prawid³owego dzia³ania projektu potrzebne s¹ dodatkowe pliki wsadowe (np. dane w arkuszu kalkulacyjnym), stwórz nowy katalog `data` i dodaj je tam. **Nie commituj plików powsta³ych w czasie kompilacji do repozytorium.**
- Je¿eli kompilujesz w oparciu o CMake'a, pliki Ÿród³owe s¹ automatycznie wykrywane w katalogu `src`, nie musisz rêcznie modyfikowaæ pliku `CMakeLists.txt`.
- Postaraj siê zachowaæ schludne formatowanie kodu. Pomocny mo¿e okazaæ siê przyk³adowo podany plik `.clang-format`.
- Postaraj siê zachowaæ konwencjê nazewnictwa pól klas, tj. prefiks "`m_`" albo sufiks "`_`".
- Postaraj siê zachowaæ wybran¹ konwencjê nazewnictwa zmiennych, klas i funkcji, np. "`NazwaKlasy`", "`nazwaFunkcji`", "`nazwa_zmiennej`". Nie jest wa¿ne jakiej konwencji u¿yjesz - wa¿ne jest, aby stosowaæ j¹ konsystentnie.

