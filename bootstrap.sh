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
  $CURL --basic --data-binary "@${file}" --silent \
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

cd demo

# Create verbs
echo "Creating verbs"
cd verbs
for verb in *; do
  if [ -f $verb ]; then
    echo -n "$verb "
    post_file $verb "api/iflib/verb"
  fi
done
cd ..

echo
echo
echo "Creating adverbs"
cd adverbs
for adverb in *; do
  if [ -f $adverb ]; then
    echo -n "$adverb "
    post_file $adverb "api/iflib/adverb"
  fi
done
cd ..

echo
echo

# Create domains
cd domains
for domain in *; do
  if [ -d $domain ]; then
    echo "Creating domain '$domain'"
    if [ -f ${domain}.json ]; then
      post_file ${domain}.json "api/worldlib/domain"
    else
      post_content "{\"name\": \"$domain\"}" "api/worldlib/domain"
    fi

    cd $domain
    for area in *; do
      if [ -d $area ]; then
        echo "  Creating area '$area'"
        if [ -f ${area}.json ]; then
          post_file ${area}.json "api/worldlib/area/$domain"
        else
          post_content "{\"name\": \"$area\"}" "api/worldlib/area/$domain"
        fi
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
        if [ -d terrains ]; then
          echo "    Creating terrains"
          cd terrains
          for terrain in *; do
            if [ -f $terrain ]; then
              echo "      Creating terrain '$terrain'"
              post_file $terrain "api/worldlib/terrain/$domain/$area/$terrain"
            fi
          done
          cd ..
        fi
        if [ -d paths ]; then
          echo "    Creating paths"
          cd paths
          for path in *; do
            if [ -f $path ]; then
              echo "      Creating path '$path'"
              post_file $scene "api/worldlib/path/$domain/$area/$path"
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
