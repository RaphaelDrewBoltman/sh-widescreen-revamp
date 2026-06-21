# Set Working Directory
Split-Path $MyInvocation.MyCommand.Path | Push-Location
[Environment]::CurrentDirectory = $PWD

Remove-Item "$env:RELOADEDIIMODS/sonicheroes.guis.widescreenrevamp/*" -Force -Recurse
dotnet publish "./sonicheroes.guis.widescreenrevamp.csproj" -c Release -o "$env:RELOADEDIIMODS/sonicheroes.guis.widescreenrevamp" /p:OutputPath="./bin/Release" /p:ReloadedILLink="true"

# Restore Working Directory
Pop-Location