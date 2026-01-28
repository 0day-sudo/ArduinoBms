#!/bin/bash

# 1. Alle Änderungen sammeln
git add .

# 2. Den Commit ausführen (Fragt dich nach einer Nachricht)
echo "Enter commit message:"
read message
git commit -m "$message"

# 3. Zum ersten Repo pushen (origin)
echo "Pushing to origin..."
git push origin master

# 4. Zum zweiten Repo pushen (second-remote)
echo "Pushing to second-remote..."
git push second-remote master

echo "Done! Everything is synced."