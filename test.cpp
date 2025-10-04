std::string query = R"(
    query Poe2PassiveTreeQuery($input: Poe2UserGeneratedDocumentInputBySlug!) {
      game: poe2 {
        documents {
          userGeneratedDocumentBySlug(input: $input) {
            data {
              data {
                buildVariants {
                  values {
                    passiveTree {
                      mainTree {
                        selectedSlugs
                        priorityList {
                          slug
                          name
                          type
                          iconURL
                          description
                        }
                      }
                      set1Tree { selectedSlugs }
                      set2Tree { selectedSlugs }
                      ascendancyTree {
                        selectedSlugs
                        priorityList {
                          slug
                          name
                          type
                          iconURL
                          description
                        }
                      }
                      attributeNodes {
                        attribute
                        nodeSlug
                      }
                      jewels {
                        iconURL
                        isUnique
                        jewelSlug
                        nodeSlug
                        prefixSlugs
                        suffixSlugs
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
)";

