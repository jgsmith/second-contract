#! /bin/sh

#
# This script will load the Default domain with some starting templates
# and scenes. You should run it from the directory it's in within the
# distribution since it uses relative paths to get to the JSON files it
# uses to create the templates and starting scenes.
#

#
# Change the following to the username and password of your first admin
# account.
#
USERNAME="username"
PASSWORD="password"

#
# Change the following to the path of your curl program
#
CURL=/usr/bin/curl

#
# Change the following to the second binary port and IP address at which
# the game is listening. The defaults here match the defaults in the
# mudlib as distributed, so if you haven't changed the settings in the
# mudlib, you can leave this alone.
#

IP="localhost"
PORT="6050"

    #####################################################
    #                                                   #
    # YOU SHOULDN'T NEED TO CHANGE ANYTHING BELOW HERE. #
    #                                                   #
    #####################################################

post_file() {
  file=$1
  url=$2
  $CURL --basic --data-binary "@${file};type=application/json" --silent \
     -H "Content-Type: application/json" \
     --show-error --user "${USERNAME}:${PASSWORD}" \
     http://${IP}:${PORT}/${url}
}

post_content() {
  content=$1
  url=$2
  echo "$content" \
  | $CURL --basic --data-binary "@-" \
     -H "Content-Type: application/json" \
     --show-error --user "${USERNAME}:${PASSWORD}" \
     http://${IP}:${PORT}/${url}
}

# Create Default domain
cd json

for domain in *; do
  if [ -d $domain ]; then
    echo "Creating domain '$domain'"
    post_content "{\"name\": \"$domain\"}" "api/worldlib/domain"

    cd $domain
    for area in *; do
      if [ -d $area ]; then
        echo "  Creating area '$area'"
        post_content "{\"name\": \"$area\"}" "api/worldlib/area/$domain"
        cd $area
        if [ -d wards ]; then
          cd wards
          for ward in *; do
            if [ -d $ward ]; then
              echo "    Creating template category '$ward'"
              post_content "{\"name\": \"$ward\"}" "api/worldlib/ward/$domain/$area"

              cd $ward
              for thing in *; do
                if [ -f $thing ]; then
                  post_file $thing "api/worldlib/thing/$domain/$area/$ward/$thing"
                fi
              done
              cd ..
            fi
          done
          cd ..
        fi
        if [ -d scenes ]; then
          echo "    Creating scenes"
          cd scenes
          for scene in *; do
            if [ -f $scene ]; then
              echo "      Creating scene '$scene'"
              post_file $scene "api/worldlib/scene/$domain/$area/$scene"
            fi
          done
          cd ..
        fi
        cd ..
      fi
    done

    cd ..
  fi
done
