## Dungeon Master

#### Pré-requis
- Outils : GNU Make et g++ (sous Windows, MinGW est un moyen d'avoir tout ça);
- Libs : SDL2 et SDL2_image. Probablement plus tard, SDL2_mixer.
- Pilote graphique : Capable d'un Core Profile OpenGL 3.3 ou plus.

#### Plus-value
- Y'a pas besoin de GLEW parce qu'il est inclus dans les sources, et non comme bibliothèque externe.
- Y'a pas besoin de CMake parce que Make c'est très bien.
- Grâce à la SDL2 (vs. SDL1 utilisée par GLimac), le programme choisit de tenter de créer un contexte OpenGL Core et Debug à la fois, ce qui vous donne souvent une version élevée (même sur les PCs défavorisés) et affiche des avertissements ou infos concernant votre utilisation d'OpenGL.
- Testé sous Linux. Marche très très probablement sous Windows et OS X si vous avez bien les pré-requis.

#### Est-ce que c'est ta forme finale ?
Non. Le code sera mieux commenté, en français, et réarrangé pour plus de clarté, avant de passer à la suite.
