# FAangband: legacy monster HP notation audit

Audit date: 2026-11-14. Scope: [Draft PR #146](https://github.com/mystudybook/angbandroid/pull/146), batch 268.

## Finding

The bundled `variant_lib/faangband/gamedata/monster.txt` contains three `hit-points:` fields in legacy dice notation. The current upstream [NickMcConnell/FAangband monster.txt](https://github.com/NickMcConnell/FAangband/blob/main/lib/gamedata/monster.txt) contains the **same three values** and the same names; these are not Android-port-specific edits.

| Monster | Shipped field | Current parser stores as `avg_hp` | Mean if the field were interpreted as dice |
| --- | --- | ---: | ---: |
| petty-dwarven rogue | `6d9` | 6 | 30 |
| gazing eye | `4d9` | 4 | 20 |
| animist | `10d20` | 10 | 105 |

The last column is **a mathematical comparison, not a proposed balance change**. The runtime does not roll the dice text.

## Verified production behavior

- `app/src/main/cpp/faangband/mon-init.c` registers the directive as `hit-points int hp` and assigns `parser_getint(p, "hp")` to `race->avg_hp`. The [upstream parser](https://github.com/NickMcConnell/FAangband/blob/main/src/mon-init.c) uses the same registration.
- `app/src/main/cpp/faangband/parser.c` uses `strtol(tok, &z, 0)`; its integer branch rejects an input only when `z == tok`. It does **not** reject a suffix, so `6d9`, `4d9`, and `10d20` yield numeric prefixes 6, 4, and 10.
- `mon-make.c::mon_hp()` subsequently generates non-unique monster HP around those `avg_hp` values. Thus **6 / 4 / 10 are parsed base values, not guaranteed final HP at spawn**.
- The three records are non-unique monsters. They have no currently identified `int16_t` range risk. The batch 267 storage clamp is independent of this finding.

## Decision for this draft PR

**Do not change the bundled monster data or the general integer parser in PR #146.** The same values are present upstream, and changing the three averages to 30, 20, and 105 would materially increase those monsters' health and change gameplay. Making all integer parser fields reject trailing characters would instead make the current bundled monster data fail to load.

A balance or upstream-data decision should be separate from UTF-8 / HP storage safety work. If this is later changed, verify the intended monster strength with upstream maintainers or historical balance evidence, update the data/parser deliberately, and adjust the corresponding regression expectations.

## Regression and follow-up

`tests/test_faangband_mon_damage_hp_source.py` now checks the exact three monster names and their raw values, confirms the parser's prefix-only behavior, and guards against extra unreviewed `NdM` fields. It also retains the existing HP range and storage checks.

This audit is **source-level**; it does not claim to have run the full game parser or Android runtime. Repository-wide Python/C99 verification and Android NDK/device testing remain outstanding.
