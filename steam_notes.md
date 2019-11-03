# Notes about Accessing Steam Images

## Traditional 460x215

```
http://cdn.akamai.steamstatic.com/steam/apps/{APP_ID}/header.jpg
https://steamcdn-a.akamaihd.net/steam/apps/{APP_ID}/header.jpg
```

## 616x353

I'm not quite sure where these are being used, but they're available.

```
https://steamcdn-a.akamaihd.net/steam/apps/{APP_ID}/capsule_616x353.jpg
```

 - This format doesn't seem to work with 460x215 images

## Box Art Sized

```
https://steamcdn-a.akamaihd.net/steam/apps/{APP_ID}/library_600x900.jpg
```

- Neither of the other resolutions seems to work with the `library_` prefix.
- As the source says, using `header.jpg` gets the 460x215 image.

Source: https://gaming.stackexchange.com/questions/359614/is-there-a-way-to-download-the-box-art-for-games
