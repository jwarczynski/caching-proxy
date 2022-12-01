# HTTP proxy z pamięcią podręczną

Niniejszy projekt realizuje serwer pośredniczący HTTP, wyposażony w pamięć podręczną.
Jego głównym zadaniem jest redukowanie ruchu sieciowego, poprzez:
* obsługę żądań, które już wcześniej zostały zapisane w pamięci podręcznej,
* redukowanie żądań o ten sam zasób, które jednocześnie przechodzą przez sieć – przepuszczone zostanie tylko jedno, a odpowiedź rozesłana do wszystkich klientów.

Serwer proxy będzie uwzględniał, oprócz adresu URL zasobu, również nagłówki takie jak `ETag` oraz `Cache-Control`, aby możliwie dokładnie i wydajnie obsługiwać zapytania przechodzące przez sieć.

## Instrukcja uruchomienia
Sposób uruchomienia:
```
make run
```

Skompiluje to projekt, a następnie uruchomi serwer proxy.