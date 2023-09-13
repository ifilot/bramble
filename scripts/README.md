# Scripts

This folder contains a number of scripts used for post-processing.

## similarity_plot.py

The Python script `similarity_plot.py` reads the output of `bramble` and produces a
similarity plot. To use this file for your own work, adjust the section under
`def main()` by the following

```python
    mhsn = read_similarity_analysis_file('<sa_file.txt>')
    pattern, names = read_pattern_analysis('<pa_file.txt>')
    fig, ax = plt.subplots(1, 1, dpi=144, figsize=(12,12))
    plot_matrix(ax, mhsn, names)
```

wherein `<sa_file.txt>` and `<pa_file.txt>` are replaced by the output files of
the similarity analysis and the pattern analysis, respectively.

## Blender

In the Blender folder, one can find a Blender source file that hosts a short Python
script to produce a rendering of a nanoparticle wherein the different types of
atoms, as identified using `bramble`, are color coded.
