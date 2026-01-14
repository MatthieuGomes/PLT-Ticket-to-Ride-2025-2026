# TUI Navigation / Navigation TUI

## English
- `Tab`: cycle focus (Command -> Map -> Info). The focused panel is highlighted.
- `Shift+Tab`: cycle focus backwards
- `m`: switch to Map view (focus must be Map)
- `p`: switch to Player view (focus must be Map)
- Arrow keys: move station selection in Map view (focus must be Map); scroll InfoPanel (focus must be Info)
- `PageUp` / `PageDown`: scroll InfoPanel history (focus must be Info)
- Type text: goes into CommandInput (focus must be Command)
- `Enter`: submit a command (`exit` closes the TUI)
- `Backspace` / `Delete`: remove the last character
- `Ctrl+C`: quit immediately
- Input length is limited to 50 characters

Notes:
- The map layout is loaded from `ressources/europe_layout.txt`. Edit that file to move station labels.

## Francais
- `Tab` : changer le focus (Commande -> Carte -> Info). Le panneau actif est mis en evidence.
- `Shift+Tab` : changer le focus en sens inverse
- `m` : basculer vers la vue Carte (focus sur Carte)
- `p` : basculer vers la vue Joueur (focus sur Carte)
- Fleches : deplacer la selection en vue Carte (focus sur Carte) ; defiler l'InfoPanel (focus sur Info)
- `PageUp` / `PageDown` : defiler l'InfoPanel (focus sur Info)
- Saisie : texte dans CommandInput (focus sur Commande)
- `Entree` : envoyer la commande (`exit` ferme la TUI)
- `Retour arriere` / `Suppr` : enlever le dernier caractere
- `Ctrl+C` : quitter immediatement
- Longueur maximale de saisie : 50 caracteres

Notes :
- La mise en page de la carte est lue depuis `ressources/europe_layout.txt`. Modifiez ce fichier pour deplacer les stations.
