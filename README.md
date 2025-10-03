# Thirsty - (Liquid Shader and Simulation)

In May 2020 the game Half-Life Alyx received an update which made the previously opaque bottles, filled with realistic looking liquid:

![Bottle with liquid in Half-Life Alyx](https://clan.steamstatic.com/images/36225228/ebd09ce02b4b9b7c3d59eb442ee6afe22f20d291.gif)

The shader have also been implemented in other Source 2 titles such as Counter-Strike 2:

![Molotov cocktail in Counter-Strike 2](assets/counter-strike2.gif)

Although the effect looks convincing it is simply clever shader tricks and no real water simulation. This comes with limitations to how the liquid can behave. Could for example an hourglass filled with liquid-like sand be faked in the same manner? Probably not.

This project aims to replicate some of the convincing effects of the liquid shader in Half-Life Alyx. It also aims to implement more accurate liquid simulations which might be able to handle the other case.