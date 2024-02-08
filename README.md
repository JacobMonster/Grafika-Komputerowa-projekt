# Interstellar Race
"Interstellar Race" to symulator wyścigu w przestrzeni kosmicznej. Zadaniem gracza jest dotrzeć do mety przed botami, zaliczając po kolei punkty kontrolne (checkpointy). Punkt kontrolny to obręcz ze strzałką w środku. Jest on zwrócony w stronę następnego checkpointu, aby gracz mógł łatwiej dostosować swój tor lotu do trasy wyścigu. Po ukończeniu wyścigu zostaje wyświetlone podium.

![start](https://github.com/JacobMonster/Grafika-Komputerowa-projekt/assets/115994083/fb94213e-ebd2-4361-b35a-b1fe18c276b6)

## Checkpointy
W kodzie określiliśmy pewien zestaw wektorów określających współrzędne 'xyz' dla checkpointów. Punkt kontrolny jest generowany w położeniu określonym przez pierwszy wektor zestawu, a następnie po osiągnięciu pewnych współrzędnych przez nasz statek (inna funkcja sprawdza, czy położenie statku jest równe położeniu obecnego checkpointu z pewnym marginesem błędu tak, aby nie trzeba było przelatywać idealnie przez środek punktu kontrolnego) punkt kontrolny pobiera następny wektor i zmienia swoje położenie. Używamy także funkcji, która oblicza macierz rotacji punktu kontrolnego tak, aby był on zwrócony w stronę swoich następnych współrzędnych. Także wystarczy zmienić wcześniej określone współrzędne 'xyz' a checkpoint'y obrócą się same.

![race checkpoints](https://github.com/JacobMonster/Grafika-Komputerowa-projekt/assets/115994083/479b7337-d737-4d6d-88d6-eeb7679d375c)

## Boty
Do poruszania się botów użyliśmy algorytmu Catmull-Rom Spline, który wyznacza nam krzywą, po której poruszają się nasi przeciwnicy. Jako argumenty ta funkcja przyjmuje kolejne współrzędne punktów kontrolnych, także ich trasa nie jest predefiniowana. Używaną pochodną od krzywej w punkcie, w którym znajduje się bot, możemy określić jego macierz rotacji tak, aby jego lot wyglądał płynnie i realistycznie.

## Planety
Używając Teksturowania i Mapowania Normalnych w naszym układzie słonecznym generujemy kilkadziesiąt planet, które są oświetlone w sposób zależny od ich położenia względem źródła światła, czyli w naszym przypadku słońca. W świecie znajduje się również czerwona planeta, na której możemy zauważyć efekt Oświetlenia Punktowego i Cieniowania Phonga. Obok niej znajdują się dwie planety przy generowaniu, których zastosowaliśmy PBR, jak widać, jedna z nich mocniej odbija światło, gdyż jest bardziej metaliczna. W pobliżu poprzednich sfer można zauważyć także inny rodzaj Ziemi, zastosowaliśmy na niej Multitexturing, tekstura chmur została nałożona na teksturę planety.

![PBR](https://github.com/JacobMonster/Grafika-Komputerowa-projekt/assets/115994083/5ddb9385-8697-4de8-bbc9-095bbc15fe97)


### Sterowanie:
W – przód
S – tył
A – obrót w lewo
D – obrót w prawo
I – góra
J – przesunięcie w lewo
K – dół
L – przesunięcie w prawo

### Start wyścigu: G

### Tryb eksploracji (dostępny po ukończeniu wyścigu): B

## Skład grupy: Jakub Chmielecki, Mikołaj Mazur, Tobiasz Przybylski
