# Thirsty - (Liquid Shaders and Simulation)

## Participants:

- Anton Nilsson, antni004@student.liu.se

## Description

In May 2020 the game Half-Life Alyx received an update which made the previously opaque bottles, filled with realistic looking liquid:

![Bottle with liquid in Half-Life Alyx](https://clan.steamstatic.com/images/36225228/ebd09ce02b4b9b7c3d59eb442ee6afe22f20d291.gif)

The shader have also been implemented in other Source 2 titles such as Counter-Strike 2:

![Molotov cocktail in Counter-Strike 2](assets/counter-strike2.gif)

Although the effect looks convincing it is simply clever shader tricks and no real water simulation. This comes with limitations to how the liquid can behave. Could for example an hourglass filled with liquid-like sand be faked in the same manner? Probably not.

This project aims to replicate some of the convincing effects of the liquid shader in Half-Life Alyx. It also aims to implement more accurate liquid simulations which might be able to handle the other case.

## Will do:

- [x] Transparency.
- [x] Bottle models.
- [ ] Refraction through the glass.
- [ ] Faked liquid using a plane and sinus curves or harmonic functions.
- [x] User input to apply forces.
- [ ] ~~More realistic liquid surface using Navier-Stokes equations.~~
- [ ] ~~Particle based liquid.~~

## Might do:

- [ ] Apply forces through gyroscope and accelerometer sensors.
- [ ] Animated textures.
- [ ] Faked bubbles
- [ ] Foam.
- [ ] Snow globe.
- [ ] ~~Hourglass~~.
- [ ] ~~Lava lamp~~.

## Halftime milestone

I would like to be done with most of my will do's by the halftime point. The one which might be left would be particle based liquid since I think that will be challenging. 

During the second part of the period I would instead like to focus on some of the things which I have listed as might do's, since I think a lot of them could be cool.

## Difficulty evaluation

I think most of the faked liquid tricks will be quite simple to implement. The sinus curves/harmonic functions might however require a lot of fine tuning to make them look convincing. 

For Navier-Stokes I have no reference for if it would be medium or advanced. For the particle based I know it will be advanced since it will require handling a lot of particle in order to make good.