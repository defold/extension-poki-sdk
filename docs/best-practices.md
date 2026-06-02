---
title: Best practices when building for the web
brief: This is a practical guide for Defold developers building for the web
---

# Best practices when building for the web

This is a practical guide for Defold developers building for the web. Pay attention to each of the topics in this article as they are crucial to your success on Poki and the web in general.


## Quick summary

If there's one thing to remember from this guide, it's this: __The best-performing games on Poki are designed for instant fun__.

That means:

* Fast loading (<20MB ideally)
* Gameplay within seconds
* Simple, satisfying gameplay loops
* Excellent controls on both desktop and mobile
* Clear visuals and readable UX
* Replayability over complexity

Web players decide very quickly whether they want to stay. The strongest games remove friction, create immediate interaction, and make players think: “One more round.”


### The web is a different playground

The web behaves differently from mobile, console, or PC. Players arrive fast, decide fast, and leave fast. At Poki, we see thousands of games enter testing pipelines every year. The games that ultimately succeed are rarely the ones with the largest feature lists or deepest progression systems. Instead, the strongest performers are usually the games that understand web player behavior exceptionally well.

A successful web game creates clarity immediately. It loads quickly, feels responsive within seconds, and gives players something satisfying to do almost instantly. On the web, the game starts the moment the page opens.


### Session 0 is everything

One of the biggest differences between mobile and web is the importance of the first session. On mobile, developers can often rely on long-term progression systems, onboarding funnels, or retention mechanics that unfold over days or weeks. Web players behave differently. Many sessions are spontaneous and curiosity-driven. The first minute matters enormously.

Successful Poki games focus on immediate interaction, low friction, clear goals, satisfying feedback, and quick understanding. Games with long intros, dialogue-heavy tutorials, complex menus, or slow unlock systems generally struggle more on web.


### Continuous gameplay performs best 🔁

Games that perform well on Poki usually keep the player actively engaged almost all the time. Whether the player is dodging enemies, matching tiles, drifting through corners, aiming shots, solving puzzles, surviving waves, or stacking combos, the important part is that the interaction feels continuous and satisfying.

Replayability matters enormously. The strongest games often create a "one more try" feeling where players naturally want to restart, improve, or continue.


### File size is one of the biggest success factors

File size directly impacts whether players even start the game. On web, loading speed strongly affects Conversion to Play (C2P). If a game loads too slowly, players simply move on to another title.

As a general guideline, games should aim for under 20MB total size. Smaller is always better. Developers should aggressively optimize textures, audio, unused assets, dependencies, startup processes, and shaders. For Defold developers specifically, this is a major opportunity. [Learn more about how to optimze game size in our optimization manual](https://defold.com/manuals/optimization-size).

Defold's lightweight runtime and HTML5-focused tooling align very well with browser player expectations.


### Controls need to feel great everywhere

Web players arrive from many different devices. The best-performing games feel smooth and intuitive everywhere. Games should respond instantly, avoid overly small UI elements, support touch naturally, and work well on both desktop and mobile.


### Simplicity usually beats complexity

Web players generally respond better to clear, lightweight systems than complicated economies or progression structures. Rewarded ads typically work best when they feel like a second chance, a revive, an optional reward, or a bonus attempt. Heavy monetization layers or complex store systems often create friction instead of improving engagement.


### Readability matters more than realism

Players browsing web platforms make extremely fast visual decisions. Strong-performing games usually communicate their appeal immediately through bold visuals, readable gameplay, clear interactions, recognizable gameplay moments, and clean UI. Visual clarity is often more important than visual complexity.


### Support both landscape and portrait

Most games on Poki will be played both on desktop and mobile. It is important to design a game which can be played in both landscape mode on desktop and in portrait mode on mobile. Supporting portrait mode on mobile also means that banner ads can be used.


### What Poki looks at during testing

The most important metrics during testing include:

* Average Playtime: 5+ minutes
* Very Strong Playtime: 7–10+ minutes
* Conversion to Play: 65%+
* File Size: Under 20MB

In general, strong playtime suggests replayability, strong conversion suggests low friction, and small file sizes improve accessibility globally.


### The biggest mistake developers make

The most common mistake is treating web as simply another export target. The strongest teams instead approach web as its own platform with different player behavior, session patterns, technical constraints, and design opportunities.

A simple question often separates successful web games from unsuccessful ones: "__Can the player start having fun within ten seconds?__"


### Final takeaway

The games that stand out on Poki are usually not the most technically complex. They are the games that remove friction, respect the player's time, load quickly, feel satisfying immediately, and create strong replayability. Web success comes from understanding player behavior first, and building around that reality from the very beginning.

For even more tips and recommendations head over to the [Poki Developer guide](https://developers.poki.com/guide).


[Back to the Poki SDK integration guide](..)