# TUI Navigation / Navigation TUI

## English
- `m`: switch to Map view (only when the input line is empty)
- `p`: switch to Player view (only when the input line is empty)
- Arrow keys: move station selection in Map view (same column index for up/down, same row for left/right)
- `PageUp` / `PageDown`: scroll InfoPanel history
- Type text: goes into the CommandInput line
- `Enter`: submit a command (`exit` closes the TUI)
- `Backspace` / `Delete`: remove the last character
- `Ctrl+C`: quit immediately
- Input length is limited to 50 characters

Notes:
- The map layout is loaded from `ressources/europe_layout.txt`. Edit that file to move station labels.

## Francais
- `m` : basculer vers la vue Carte (ligne de commande vide)
- `p` : basculer vers la vue Joueur (ligne de commande vide)
- Fleches : deplacer la selection en vue Carte (meme colonne pour haut/bas, meme ligne pour gauche/droite)
- `PageUp` / `PageDown` : defiler l'InfoPanel
- Saisie : texte dans la ligne de commande
- `Entree` : envoyer la commande (`exit` ferme la TUI)
- `Retour arriere` / `Suppr` : enlever le dernier caractere
- `Ctrl+C` : quitter immediatement
- Longueur maximale de saisie : 50 caracteres

Notes :
- La mise en page de la carte est lue depuis `ressources/europe_layout.txt`. Modifiez ce fichier pour deplacer les stations.
