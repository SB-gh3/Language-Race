with import <nixpkgs> {};

mkShell 
{
  buildInputs = 
  [
    python313Packages.pympler
    ihaskell
  ];
}