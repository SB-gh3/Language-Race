import System.Environment (getArgs)
import System.IO (hPutStrLn, stderr)
import Data.List (foldl', sortBy, scanl')
import Data.Ord (comparing)
import Data.Time.Clock (getCurrentTime, diffUTCTime)
import Text.Printf (printf)
import GHC.Stats
import Data.Word
import Data.Char (isSpace)
import Data.List (dropWhileEnd)

getMemoryStats :: IO Word64
getMemoryStats = do
    stats <- getRTSStats
    return (allocated_bytes stats)

lev :: String -> String -> Int
lev s1 s2 = last finalRow
  where
    n = length s2
    initialRow = [0..n]

    -- Fold over the characters of the first string (s1). The accumulator `prevRow`
    -- holds the previously calculated row of distances.
    finalRow = foldl' nextRow initialRow (zip [1..] s1)

    -- | Calculates the next row of distances based on the previous row and a character from s1.
    nextRow :: [Int]      -- ^ The previous row of distances (v0 in the C++ code).
            -> (Int, Char) -- ^ A tuple of (current row index, character from s1).
            -> [Int]       -- ^ The newly computed row of distances (v1).
    nextRow prevRow (rowIndex, c1) =
      -- `scanl'` is used to efficiently build the new row. It's similar to a fold
      -- but returns all intermediate results, which is perfect for creating a list.
      -- The accumulator `leftCost` is the value of the cell to the left in the current row.
      scanl' (\leftCost (c2, aboveLeftCost, aboveCost) ->
               let
                   deletionCost     = aboveCost + 1
                   insertionCost    = leftCost + 1
                   substitutionCost | c1 == c2  = aboveLeftCost
                                    | otherwise = aboveLeftCost + 1
               in minimum [deletionCost, insertionCost, substitutionCost]
             ) rowIndex (zip3 s2 prevRow (drop 1 prevRow))

main :: IO ()
main = do
    startTime <- getCurrentTime --Start Time
    startMem <- getMemoryStats --Start Memory

    -- Get args
    args <- getArgs
    case args of
        -- args names
        [target, inputFile, outputFile] -> do
            -- Read the entire input file
            contents <- readFile inputFile
            let fileLines = lines contents

            -- Create a list of tuples
            -- dropWhileEnd isSpace removes line terminating characters, which break the program
            let order = [(dropWhileEnd isSpace line, lev target (dropWhileEnd isSpace line)) | line <- fileLines]

            -- Sort the list of tuples based on the distance (the second element of the tuple).
            -- `comparing snd` creates a comparison function that looks at the second element (`snd`).
            let sorted = sortBy (comparing snd) order

            -- Format the sorted list back into a single string for writing to a file.
            -- `map` applies the formatting function to each tuple.
            -- `unlines` joins the resulting list of strings with newlines.
            let formatTuple (line, dist) = line ++ " " ++ show dist
            let outputContent = unlines $ map formatTuple sorted

            -- Write the result to the specified output file
            writeFile outputFile outputContent

            -- Calculate total time
            end <- getCurrentTime
            let endTime = diffUTCTime end startTime
            let timeTotal = realToFrac endTime :: Double

            --End Memory
            finalMem <- getMemoryStats
            let memTotal = finalMem - startMem
            
            putStrLn "\nHaskell:"
            printf "Execution time: %.3fs\n" timeTotal
            printf "Memory increase: %.3f MiB\n" (fromIntegral memTotal / (1024 * 1024 * 1024) :: Double)